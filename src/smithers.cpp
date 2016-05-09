#include "smithers.h"

#include <iostream>
#include <sstream>
#include <m2pp.hpp>
#include <random>
#include <zmq.hpp>
#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    
    s[len] = 0;
    std::string randy(s);
    return randy;
}

} // close anon namespace

namespace smithers{


void Smithers::await_registered_players(const int max_players){
    std::cout << "await_registered_players().." << std::endl;

    for (int i=0; i<max_players; ++i){
        m2pp::connection conn("YOYOYO", "tcp://127.0.0.1:9997", "tcp://127.0.0.1:9996");
        m2pp::request req = conn.recv();

        if (req.disconnect) {
            std::cout << "== disconnect ==" << std::endl;
            --i;
            continue;
        }

        std::ostringstream log_request;
        log_request << "<pre>" << std::endl;
        log_request << "SENDER: " << req.sender << std::endl;
        log_request << "IDENT: " << req.conn_id << std::endl;
        log_request << "PATH: " << req.path << std::endl;
        log_request << "BODY: " << req.body << std::endl;
        for (std::vector<m2pp::header>::iterator it=req.headers.begin();it!=req.headers.end();it++) {
            log_request << "HEADER: " << it->first << ": " << it->second << std::endl;
        }
        log_request << "</pre>" << std::endl;

        std::cout << log_request.str();

        std::ostringstream name;
        name << "Player" << i;
        
        Player new_player(name.str(), gen_random(20), i);
        
        std::ostringstream player_info;
        player_info << "{ "
                    << "\"name\":\"" << new_player.m_name << "\","
                    << "\"cash\":10000,"
                    << "\"key\":\""<< new_player.m_hash_key<<"\""
                    << " }" << std::endl;

        conn.reply_http(req, player_info.str());
        m_players.push_back(new_player);
    }

}

void Smithers::publish_to_all(const std::string& message) const{
    std::cout << "publish_to_all " << message << std::endl;
    // implement sending out zmq messages and also websockets
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    publisher.bind("tcp://127.0.0.1:9950");

    while (true){   
        zmq::message_t zmq_message(message.begin(), message.end());
        publisher.send(zmq_message);
    }
}

void Smithers::play_game(){
    std::cout << "play_game" << std::endl;
}



void Smithers::print_players(){
    publish_to_all("You're all watching the game");
    
};
} // smithers namespace