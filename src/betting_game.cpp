#include "betting_game.h"

#include "player_utils.h"

namespace smithers{

namespace {

PlayerMove_t process_a_raise_bet(int this_bet, int available_chips,
                                int min_raise, int last_bet)
{
    PlayerMove_t processed_move = {ERROR,-1,-1,-1};

    // 0. Check for lying
    if (this_bet >= available_chips)
    {
         this_bet = available_chips; 
    }

    // 1. Check raises
    int new_raise = this_bet - last_bet;
    if (new_raise >= min_raise)
    {
        // raise > min raise -> a real RAISE
        processed_move.move_type = RAISE; 
        
        processed_move.new_min_raise = new_raise;
        processed_move.amount = this_bet;
        processed_move.last_bet = this_bet; 
    }
    else if (new_raise >= 0)
    {  
        // raise < min raise -> CALL
        processed_move.move_type =  CALL; 

        processed_move.amount = last_bet;
        processed_move.last_bet = last_bet; 
        processed_move.new_min_raise = min_raise;

    }
    else if(new_raise < 0)
    {
        // this bet < last bet -> FOLD
        processed_move.move_type =  FOLD; 

        processed_move.amount = -1;
        processed_move.last_bet = last_bet; 
        processed_move.new_min_raise = min_raise;
    }
 
    // 2. Mark if its effectively an all in
    if (this_bet == available_chips && processed_move.move_type != FOLD)
    {
        processed_move.move_type =  ALL_IN;
    }
    return processed_move;
}

PlayerMove_t process_a_call_bet(int available_chips, int min_raise, int last_bet)
{
    if (last_bet >= available_chips)
    {
        // this call >= availab chips -> ALL IN 
        PlayerMove_t processed_move = {ALL_IN, available_chips, last_bet, min_raise};
        return processed_move;
    }
    else
    {   
        // this call <= availab chips -> CALL
        PlayerMove_t processed_move = {CALL, last_bet, last_bet, min_raise};    
        return processed_move;
    }
}

PlayerMove_t process_a_fold_bet(int min_raise, int last_bet)
{
    PlayerMove_t processed_move = {FOLD, 0, last_bet, min_raise};
    return processed_move;
} 

} // close anon namespace


BettingGame::BettingGame(m2pp::connection& listener, zmq::socket_t& publisher, std::vector<Player>& players)
    : m_listener(listener),
    m_publisher(publisher),
    m_players(players)
{
}

void BettingGame::run_betting_round(int first_betting_seat, int min_raise, int last_bet)
{
    // get nth place to left of dealer
    int to_play_seat = player_utils::get_dealer(m_players);
    for (int i=0; i<first_betting_seat; i++)
    {
        to_play_seat = player_utils::get_next_to_play(m_players,  to_play_seat);
    }

    int last_to_raise_seat = to_play_seat;

    do 
    {   
        // 1. Get Player 
        Player& next_player = m_players[to_play_seat];
        
        // 2. Get Move & Process it
        const Json::Value raw_move = get_players_move( next_player, min_raise, last_bet );
        const PlayerMove_t this_move = process_move(raw_move, next_player, min_raise, last_bet);
        do_players_move(next_player, this_move);
        
        // 3. Tell People
        Json::Value move_message = create_move_message(next_player, this_move.move_type, this_move.amount);
        publish_to_all(move_message);
        
        // 4. Is She Last to Raise?
        if (this_move.last_bet > last_bet)
        {
            last_bet = this_move.last_bet;
            last_to_raise_seat = to_play_seat; 
        }
        
        // 5. Get Next Player
        to_play_seat = player_utils::get_next_to_play(m_players,  to_play_seat);

    } while (to_play_seat != last_to_raise_seat);

    end_round_betting(); 
}

Json::Value BettingGame::get_players_move(const Player& player,
                                                int min_raise,
                                                int last_bet)
{
    // TBD: retry on failure?
    Json::Value move_request_message = create_move_request(player, get_pot_value(), last_bet);
    publish_to_all(move_request_message);

    Json::Value move = listen_and_pull_from_queue(player.m_name);
    return move;
}

PlayerMove_t BettingGame::process_move(const Json::Value& move,
                                    const Player& player,
                                    int min_raise,
                                    int last_bet) const// change these reference
{
    std::string this_move = move.get("move", "").asString();
    int this_bet = move.get("chips", "0").asInt();
    int available_chips = player.m_chips - player.m_chips_this_game;

    if (this_move == "RAISE_TO" or this_move == "RAISE")
    {
        return process_a_raise_bet(this_bet, available_chips, min_raise, last_bet);
    }
    else if (this_move == "CALL")
    {
        return process_a_call_bet(available_chips, min_raise, last_bet);
    }
    else if (this_move == "FOLD")
    {
        return process_a_fold_bet(min_raise, last_bet);
    }

    return process_a_fold_bet(min_raise, last_bet);
}

void BettingGame::do_players_move(Player& player, const PlayerMove_t& move)
{
    if (move.move_type==FOLD)
    {
        player.m_in_play_this_round = false;
    }
    else 
    {
        if (move.move_type==ALL_IN)
        {
            player.m_in_play_this_round = true;
        }
        player.m_chips_this_round = move.amount;
    }
}

void BettingGame::end_round_betting()
{
    // end by transferring round bets to game bets
    for (std::vector<Player>::iterator it = m_players.begin(); it != m_players.end(); it++)
    {
        it->m_chips_this_game += it->m_chips_this_round; 
        it->m_chips_this_round = 0;

    }
}

Json::Value BettingGame::listen_and_pull_from_queue(const std::string& player_name)
{
    while (true) // timer before sending re-request
    {
        std::cout << "pulling message from listener..." << std::endl; 
        m2pp::request req = m_listener.recv();
        std::cout << req.body << std::endl;

        if (req.disconnect)
        {
            continue;
        }
        else
        {
            Json::Value root;
            Json::Reader reader;
            bool was_success = reader.parse(req.body, root);
            std::cout << req.body << std::endl;
            m_listener.reply_http(req, "{}"); // should send back before the parse status
            
            if  (!was_success || root.get("name", "").asString() != player_name)
            {
                continue;
            }

            return root; 
        }
    }
}

void BettingGame::run_pocket_betting_round()
{
    // add blinds
    run_betting_round(3, 100, 0);
}

void BettingGame::run_flop_betting_round()
{
    run_betting_round(1, 100, 0);
}

void BettingGame::run_river_betting_round()
{
    run_betting_round(1, 100, 0);
}

void BettingGame::run_turn_betting_round()
{
    run_betting_round(1, 100, 0);
}

int BettingGame::get_pot_value() const
{
    return player_utils::get_pot_value_for_game(m_players);
}

void BettingGame::publish_to_all(const std::string& message)
{
    zmq::message_t zmq_message(message.begin(), message.end());
    m_publisher.send(zmq_message);
}

void BettingGame::publish_to_all(const Json::Value& json)
{
    std::ostringstream message;
    message << json;
    publish_to_all(message.str());
}





} // smithers
