#include "betting_game.h"

#include "player_utils.h"

namespace smithers{

namespace {

PlayerMove_t process_raise_bet(int this_bet, int available_chips,
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

PlayerMove_t process_call_bet(int available_chips, int min_raise, int last_bet)
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

PlayerMove_t process_fold_bet(int min_raise, int last_bet)
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

void BettingGame::run_betting_round(int first_betting_seat, int min_raise, int last_bet)
{
    int next_to_play_index = player_utils::get_dealer(m_players);
    
    for (int i=0; i<first_betting_seat; i++)
    { // ie 3rd from dealer with blinds, 1 if not.
        next_to_play_index = player_utils::get_next_to_play(m_players,  next_to_play_index);
    }

    std::string next_to_play_name = m_players[next_to_play_index].m_name;
    std::string last_to_raise_name = next_to_play_name;
    do 
    {
        // 1. Find next to play.        
        Player& next_player = m_players[next_to_play_index];

        // 2. Get a move & process it.
        const PlayerMove_t this_move = handle_move_from_player( next_player, min_raise, last_bet );

        // 3. Tell people about it        
        publish_to_all(create_move_message(next_player, this_move.move_type, next_player.m_chips_this_round));
        
        // 4. Adjust last to bet & bet amount
        if ( (this_move.move_type == ALL_IN && 
                  next_player.m_chips_this_round > last_bet)
            || this_move.move_type == RAISE)
        {
            last_bet = next_player.m_chips_this_round;
            last_to_raise_name = next_player.m_name; 
        }
        
        // 5. Move to next player
        next_to_play_index = player_utils::get_next_to_play(m_players,  next_to_play_index);
        next_to_play_name = m_players[next_to_play_index].m_name;

    } while (last_to_raise_name != next_to_play_name);
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

enum MoveType BettingGame::process_bets(const Json::Value& move,
                                    Player& player,
                                    int& min_raise,
                                    int& last_bet)// change these reference
{
    std::string this_move = move.get("move", "").asString();
    int this_bet = move.get("chips", "0").asInt();

    // test valid inputs here: could do this in a separate function

    // ERROR HANDLING:
    // 1. Bets more than available chips -> ALL_IN
    if (this_bet + player.m_chips_this_game >=  player.m_chips )
    {
        player.m_chips_this_round = player.m_chips - player.m_chips_this_game;
        return ALL_IN;
    }

    int available_chips = player.m_chips - player.m_chips_this_game;
    if (this_move == "RAISE_TO" or this_move == "RAISE")
    {
        return process_raise_bet(this_bet, available_chips, min_raise, last_bet).move_type;
    }
    else if (this_move == "CALL")
    {
        return process_call_bet(available_chips, min_raise, last_bet).move_type;
    }
    else if (this_move == "FOLD")
    {
        return process_fold_bet(min_raise, last_bet).move_type;
    }
    else
    {
        player.m_in_play_this_round = false;
        return FOLD;
    }

    return FOLD; // no compiler warnings

}


// enum MoveType BettingGame::handle_move_from_player(Player& player, int min_raise, int last_bet)
// {
//     publish_to_all(create_move_request(player, player_utils::get_pot_value_for_game(m_players), last_bet));
//     Json::Value move = listen_and_pull_from_queue(player.m_name);
//     enum MoveType result = process_bets(move, player, min_raise, last_bet);
    
//     return result;     
// }





PlayerMove_t BettingGame::handle_move_from_player( Player& player,
                                                        int min_raise,
                                                        int last_bet)
{
    publish_to_all(create_move_request(player, player_utils::get_pot_value_for_game(m_players), last_bet));
    Json::Value move = listen_and_pull_from_queue(player.m_name);
    
    enum MoveType result = process_bets(move, player, min_raise, last_bet);

    PlayerMove_t processed_move = { 
        result,
        player.m_chips_this_round,
        last_bet,
        min_raise,
    };
    return processed_move;
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
