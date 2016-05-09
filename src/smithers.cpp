#include "smithers.h"

#include "game.h"

#include <zmq.hpp>
#include <m2pp.hpp>

#include <iostream>
#include <sstream>
#include <random>


#if (defined (WIN32))
#include <zhelpers.hpp>
#endif


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
        publish_to_all("I'm alive");
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

void Smithers::play_game(){
    std::cout << "play_game" << std::endl;
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