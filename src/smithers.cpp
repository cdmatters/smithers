#include "smithers.h"

#include <zmq.hpp>
#include <m2pp.hpp>
#include <json/json.h>

#include <iostream>
#include <sstream>
#include <random>


// #if (defined (WIN32))
// #include <zhelpers.hpp>
// #endif


namespace {

std::string gen_random(const size_t len) {
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

void log_request(const m2pp::request& req) {
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

Smithers::Smithers():
    m_zmq_context(1),
    m_publisher(m_zmq_context, ZMQ_PUB){
        m_publisher.bind("tcp://127.0.0.1:9950");
}


void Smithers::await_registered_players(const int max_players){
    std::cout << "await_registered_players().." << std::endl;

    m2pp::connection conn("UUID_1", "tcp://127.0.0.1:9997", "tcp://127.0.0.1:9996");
    
    int seat = 1;
    while (true){
        m2pp::request req = conn.recv();

        if (req.disconnect) {
            std::cout << "== disconnect ==" << std::endl;
            continue;
        }

        log_request(req);

        std::ostringstream name;
        name << "Player" << seat;
        
        Player new_player(name.str(), gen_random(20), seat);
        
        std::ostringstream player_info;
        player_info << "{ "
                    << "\"name\":\"" << new_player.m_name << "\","
                    << "\"cash\":10000,"
                    << "\"key\":\""<< new_player.m_hash_key<<"\""
                    << " }" << std::endl;

        conn.reply_http(req, player_info.str());
        m_players.push_back(new_player);
        if (seat < max_players){
            seat++;
        } else {
            break;
        }

    }

}

void Smithers::publish_to_all(const std::string& message){
    std::cout << "ALL: " << message << std::endl;
    zmq::message_t zmq_message(message.begin(), message.end());
    m_publisher.send(zmq_message);
}

void Smithers::publish_to_all(const Json::Value& json){
    std::ostringstream message;
    message << json;
    publish_to_all(message.str());
}

void Smithers::play_game(){
    std::cout << "play_game" << std::endl;
    
    Game new_game;
    std::vector<Hand> hands = new_game.deal_hands(m_players.size()); // need to eject players
    Json::Value dealt_hands = create_dealt_hands_message(hands);
    // add blinds, set dealer
    publish_to_all(dealt_hands);

    new_game.deal_flop();
    Json::Value flop = create_table_cards_message(new_game.get_table());
    // add pot
    publish_to_all(flop);

    new_game.deal_river();
    Json::Value river = create_table_cards_message(new_game.get_table());
    // add pot
    publish_to_all(river);
    
    new_game.deal_turn();
    Json::Value turn = create_table_cards_message(new_game.get_table());
    // add pot
    publish_to_all(turn);

}

Json::Value Smithers::create_dealt_hands_message(const std::vector<Hand>& hands){
    
    Json::Value players(Json::arrayValue);
    for (size_t i=0; i<m_players.size(); ++i){
        if (!m_players[i].m_in_play){
            continue;
        }
        Json::Value player;
        player["name"] = m_players[i].m_name;
        player["chips"] = m_players[i].m_chips;
        player["hand"] << hands[i];

        players.append(player);
        
    }

    Json::Value root;
    root["type"] = "DEALT_HANDS";
    root["pot"] = 0;
    root["players"] = players;

    return root;
}

Json::Value Smithers::create_table_cards_message(const std::vector<Card>& cards){
    Json::Value card_vector(Json::arrayValue);
    for (std::vector<Card>::const_iterator c_it = cards.cbegin();
        c_it != cards.cend();
        c_it++){
        // ugly
        std::ostringstream c;
        c << *c_it;
        card_vector.append(c.str());
    } 

    Json::Value root;
    root["type"] = "DEALT_HANDS";
    root["pot"] = 0;
    root["cards"] = card_vector;

    return root;
}

void Smithers::play_betting_round(int first_to_play){
    std::string last_raise = ""; 
    std::string to_move = m_players[first_to_play].m_name;
    while (last_raise != to_move){
        // send_move_request(to_move);
        // Json::Value raw_move = listen_and_pull_from_queue(to_move);
        // Json::Value processed_move = verify_and_process_move(raw_move);
        // if (is_raise(processed_move)){
        //     last_raise = to_move;
        // } else if (is_fold(processed_move)){
        //     //fold character
        // }
        // publish_move(move);
    }
    //return if all folded or not;
}


void Smithers::print_players(){
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
} // smithers namespace