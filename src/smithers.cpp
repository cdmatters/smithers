#include "smithers.h"

#include "player_utils.h"

#include "game_runner.h"

#include <zmq.hpp>
#include <m2pp.hpp>
#include <json/json.h>

#include <iostream>
#include <sstream>
#include <random>
#include <algorithm> 


namespace {



std::string gen_random_string(const size_t len)
{
    char s[len];
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::random_device gen;

    for (size_t i = 0; i < len; ++i) {
        s[i] = alphanum[gen() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0; // null
    
    return std::string(s);
}

void log_request(const m2pp::request& req)
{
    std::ostringstream log_request;

    log_request << "<pre>" << std::endl;
    log_request << "SENDER: " << req.sender << std::endl;
    log_request << "IDENT: " << req.conn_id << std::endl;
    log_request << "PATH: " << req.path << std::endl;
    log_request << "BODY: " << req.body << std::endl;
    
    for (std::vector<m2pp::header>::const_iterator it=req.headers.cbegin();it!=req.headers.cend();it++) {
        log_request << "HEADER: " << it->first << ": " << it->second << std::endl;
    }

    log_request << "</pre>" << std::endl;

    std::cout << log_request.str();
}

} // close anon namespace

namespace smithers{

// bool result_comparator(const Result_t& r1,const Result_t& r2 ){return r1.score>r2.score;}


Smithers::Smithers():
    m_zmq_context(1),
    m_publisher(m_zmq_context, ZMQ_PUB)
{
    m_publisher.bind("tcp://127.0.0.1:9950");
}


void Smithers::await_registered_players(int max_players, int max_chips)
{
    std::cout << "await_registered_players().." << std::endl;

    m2pp::connection conn("UUID_1", "tcp://127.0.0.1:9997", "tcp://127.0.0.1:9996"); 
    
    int seat = 1;
    while (true){
        m2pp::request req = conn.recv();

        if (req.disconnect) {
            // std::cout << "== disconnect ==" << std::endl;
            continue;
        }

        log_request(req);

        Json::Value root;
        Json::Reader reader;

        bool was_success = reader.parse( req.body, root );
        if ( !was_success ){
            std::cout  << "Failed to parse configuration\n"
                       << reader.getFormattedErrorMessages();
            return;
        }
 
        std::string default_name =  "Player" + std::to_string(seat);
        std::string name = root.get("name", default_name).asString();
        


        Player new_player( name, gen_random_string(100), seat, max_chips );

        std::ostringstream resp;
        resp << create_registered_message(new_player);


        conn.reply_http(req, resp.str());
        m_players.push_back(new_player);
        if (seat < max_players){
            seat++;
        } else {
            break;
        }

    }
    m_players[0].m_is_dealer = true;

}

void Smithers::publish_to_all(const std::string& message)
{

    zmq::message_t zmq_message(message.begin(), message.end());
    m_publisher.send(zmq_message);
}

void Smithers::publish_to_all(const Json::Value& json)
{
    if (json["type"]=="WINNER"){
        std::cout << "ALL: " << json << std::endl;
    }
    std::ostringstream message;
    message << json;
    publish_to_all(message.str());
}

// void Smithers::play_game(int min_raise)
// {   
//     m2pp::connection conn("ID", "tcp://127.0.0.1:9900", "tcp://127.0.0.1:9901"); //will have to move
//     BettingGame betting_game(conn, m_publisher, m_players); 
//     CardGame card_game;
    
//     int dealer_seat = player_utils::get_dealer(m_players);
//     assign_seats(dealer_seat);

//     std::vector<Hand> hands = card_game.deal_hands( m_players.size() ); 
    
//     publish_to_all( create_dealt_hands_message( hands, m_players, dealer_seat ) );

//     betting_game.run_pocket_betting_round(min_raise);
    
//     card_game.deal_flop();
//     publish_to_all( create_table_cards_message(card_game.get_table(), player_utils::get_pot_value_for_game(m_players) ) );
//     betting_game.run_flop_betting_round(min_raise);

//     card_game.deal_river();
//     publish_to_all( create_table_cards_message(card_game.get_table(), player_utils::get_pot_value_for_game(m_players) ) );
//     betting_game.run_turn_betting_round(min_raise);
    
//     card_game.deal_turn();
//     publish_to_all( create_table_cards_message(card_game.get_table(), player_utils::get_pot_value_for_game(m_players) ) );
//     betting_game.run_river_betting_round(min_raise);

//     std::vector<Result_t> results = award_winnings( card_game.return_hand_scores() );
//     publish_to_all( create_results_message(results, m_players) );

//     reset_and_move_dealer_to_next_player();
// }

void Smithers::play_tournament()
{
    for (size_t i=0; i<m_players.size(); i++)
    {
        m_players[i].m_in_play= true;
        m_players[i].m_in_play_this_round = true;
        m_players[i].m_chips = 10000;
        m_players[i].m_chips_this_game = 0;
        m_players[i].m_chips_this_round = 0;

    } 

    int min_raise = 100;
    int hands_count = 0;
    while ( player_utils::count_active_players(m_players) > 1 )
    {
        if (hands_count % 10 == 0 )
        {
            min_raise *= 2;
        }
        GameRunner gr(m_players, m_publisher);
        gr.play_game(min_raise);
        player_utils::mark_broke_players(m_players);
        hands_count++;
    }

    players_cit_t win_it = std::find_if( m_players.begin(), m_players.end(), [](const Player p){return p.m_chips>0;} );
    publish_to_all( create_tournament_winner_message( win_it->m_name, win_it->m_chips ) );
}


// std::vector<Result_t> Smithers::award_winnings(const std::vector<ScoredFiveCardsPair_t>& scored_hands)
// {
//     std::vector<Result_t> results;

//     for (size_t i=0; i<m_players.size(); i++){   
        
//         if (!m_players[i].m_in_play || !m_players[i].m_in_play_this_round)
//         {
//             continue;
//         }
//         int seat = m_players[i].m_seat;
//         std::ostringstream cards;
//         cards <<scored_hands[seat].second;
//         Result_t r = {scored_hands[seat].first,cards.str(), i, 0};
//         results.push_back(r);
//     }

//     std::sort(results.begin(), results.end(), result_comparator);

//     for (size_t r=0; r<results.size(); r++)
//     {
//         Player& winner = m_players[results[r].player_index];
//         int winners_bet = winner.m_chips_this_game;

//         for (size_t p=0; p<m_players.size(); p++){
//             int amount = (m_players[p].m_chips_this_game >= winners_bet) ? 
//                             winners_bet : m_players[p].m_chips_this_game;
//             results[r].winnings += amount;
//             m_players[p].m_chips_this_game -= amount;
//             m_players[p].m_chips -= amount;
//         }
        
//         winner.m_chips += results[r].winnings;
//     }
        
//     return results;
// }

Json::Value Smithers::listen_and_pull_from_queue(const std::string& player_name)
{
    m2pp::connection conn("ID", "tcp://127.0.0.1:9900", "tcp://127.0.0.1:9901"); // is it smart to do this here?
    while (true){
        m2pp::request req = conn.recv();
        if (req.disconnect) 
        {
            std::cout << "== disconnect ==" << std::endl;
            continue;
        } 
        else 
        {
            Json::Value root;
            Json::Reader reader;
            bool was_success = reader.parse(req.body, root);
            if (!was_success || root.get("name", "").asString() != player_name)
            {
                continue;
            }

            conn.reply_http(req, "{}");
            return root;
        }
    }

}

void Smithers::print_players()
{
    std::ostringstream message;
    message << '[';
    for (players_cit_t it = m_players.cbegin();
        it !=  m_players.cend();
        it++)
    {

        message << '{'
         << "\"name\":\"" << it->m_name <<"\", "
         << "\"seat\":\"" << it->m_seat <<"\", "
         << "\"chips\":\"" << it->m_chips <<"\" "
         << "},";

    }
    message << "]" << std::endl;
    publish_to_all(message.str());
}

// int Smithers::assign_seats(int dealer)
// {
//     int seat = 0;
//     for (size_t i=0; i<m_players.size(); ++i)
//     {
//         int seat_no = (dealer + i + 1) % m_players.size();
//         if ( ! m_players[seat_no].m_in_play )
//         {
//             m_players[seat_no].m_seat = -1;
//         }
//         else
//         {
//             m_players[seat_no].m_seat = seat;
//             seat++;
//         }
//     }
//     return seat; // number of players
// }

// void Smithers::reset_and_move_dealer_to_next_player()
// {
//     int dealer = player_utils::get_dealer(m_players);
//     for (size_t i=0; i<m_players.size(); ++i){
//         m_players[i].m_in_play_this_round = true;
//         m_players[i].m_all_in_this_round = false;
//     }
//     int next_dealer = player_utils::get_next_to_play(m_players, dealer);
//     std::cout<< dealer<< " "<< next_dealer << std::endl;

//     m_players[dealer].m_is_dealer = false;
//     m_players[next_dealer].m_is_dealer = true;
// };



} // smithers namespace