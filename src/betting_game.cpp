#include "betting_game.h"

#include "player_utils.h"

namespace smithers{

BettingGame::BettingGame(m2pp::connection& listener, zmq::socket_t& publisher, std::vector<Player>& players)
    : m_listener(listener),
    m_publisher(publisher),
    m_players(players)
{
}

void BettingGame::run_betting_round(int first_betting_seat, int min_raise, int last_bet)
{
    int next_to_play_index = player_utils::get_dealer(m_players);
    
    for (int i=0; i<first_betting_seat; i++)
    { // ie 3rd from dealer with blinds, 1 if not.
        next_to_play_index = player_utils::get_next_to_play(m_players,  next_to_play_index);
    }

    // **TBD** Do we need these names?
    std::string next_to_play_name = m_players[next_to_play_index].m_name;
    std::string last_to_raise_name = next_to_play_name;

    do 
    {
        // 1. Find next to play.        
        Player next_player = m_players[next_to_play_index];

        // 2. Get a move & process it.
        enum MoveType result = handle_move_from_player( next_player, min_raise, last_bet );

        // 3. Tell people about it        
        publish_to_all(create_move_message(next_player, result, next_player.m_chips_this_round));
        
        // 4. Adjust last to bet & bet amount
        if ( (result == ALL_IN && 
                  next_player.m_chips_this_round > last_bet)
            || result == RAISE)
        {
            last_bet = next_player.m_chips_this_round;
            last_to_raise_name = next_player.m_name; 
        }
        
        // 5. Move to next player
        next_to_play_index = player_utils::get_next_to_play(m_players,  next_to_play_index);
        std::string next_to_play_name = m_players[next_to_play_index].m_name;

    } while (last_to_raise_name != next_to_play_name);
}

Json::Value BettingGame::listen_and_pull_from_queue(const std::string& player_name)
{
    while (true) // timer before sending re-request
    {
        m2pp::request req = m_listener.recv();
        if (req.disconnect)
        {
            continue;
        }
        else
        {
            Json::Value root;
            Json::Reader reader;
            bool was_successs = reader.parse(req.body, root);
            
            m_listener.reply_http(req, "{}"); // should send back before the parse status
            
            if  (!was_successs || root.get("name", "").asString() != player_name)
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

    if (this_move == "RAISE_TO" or this_move == "RAISE")
    {
        int new_raise = this_bet - last_bet;
        if (new_raise >= min_raise)
        {
            min_raise = new_raise;
            player.m_chips_this_round = this_bet;

            last_bet = this_bet; 
            return RAISE; // raise > min raise -> a real RAISE
        }
        else if (new_raise >= 0)
        {  
            player.m_chips_this_round = last_bet;
            return CALL; // raise < min raise -> CALL
        }
        else if(new_raise < 0)
        {
            player.m_in_play_this_round = false;
            return FOLD; // this bet < last bet -> FOLD
        }
    }
    else if (this_move == "CALL")
    {
        if (last_bet + player.m_chips_this_game > player.m_chips)
        {
            player.m_chips_this_round = player.m_chips - player.m_chips_this_game;
            return ALL_IN;
        }
        player.m_chips_this_round = last_bet;
        return  CALL; 
    }
    else if (this_move == "FOLD")
    {
        player.m_in_play_this_round = false;
        return FOLD;
    }
    else
    {
        player.m_in_play_this_round = false;
        return FOLD;
    }

    return FOLD; // no compiler warnings

}

enum MoveType BettingGame::handle_move_from_player(Player& player, int min_raise, int last_bet)
{
Json::Value move = listen_and_pull_from_queue(player.m_name);
enum MoveType result = process_bets(move, player, min_raise, last_bet);

return result;     
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
