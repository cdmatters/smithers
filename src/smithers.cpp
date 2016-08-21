#include "smithers.h"

#include "player_utils.h"
#include "game_runner.h"

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
    m_publistener("PUBLIST", "tcp://127.0.0.1:9997", "tcp://127.0.0.1:9996"),
    m_zmq_context(1),
    m_pub_socket(m_zmq_context, ZMQ_PUB)
{
    m_pub_key = "54c6755b-9628-40a4-9a2d-cc82a816345e";
    
    m_pub_socket.bind("tcp://127.0.0.1:9950");
}


void Smithers::await_registered_players(int max_players, int max_chips)
{
    std::cout << "await_registered_players().." << std::endl;

    m2pp::connection& conn = m_publistener;
    int listeners = 0;
    int seat = 0;
    while (seat < max_players)
    {
        m2pp::request req = conn.recv();

        if (req.disconnect) {
            // std::cout << "== disconnect ==" << std::endl;
            continue;
        }

        // log_request(req);

        if (req.path == "/register/" && seat < max_players)
        {
            Json::Value root;
            Json::Reader reader;

            bool was_success = reader.parse( req.body, root );
            if ( !was_success ){
                std::cout  << "Failed to parse configuration\n"
                           << reader.getFormattedErrorMessages();
                return;
            }
     
            // TBD empty string or duplicates.
            std::string default_name =  "Player" + std::to_string(seat);
            std::string name = root.get("name", default_name).asString();

            Player new_player( name, gen_random_string(100), seat, max_chips );

            std::ostringstream resp;
            resp << create_registered_message(new_player);

            conn.reply_http(req, resp.str());
            m_players.push_back(new_player);
            seat++;
        }
        else if (req.path == "/watch/")
        {
            m_pub_idents.push_back(req.conn_id);
        
            std::stringstream handshake;
            handshake << "HTTP/1.1 101 Switching Protocols\r\n" 
                  << "Upgrade: websocket\r\n" 
                  << "Connection: Upgrade\r\n"  
                  << "Sec-WebSocket-Accept: " << req.body << "\r\n"
                  << "\r\n";

            conn.reply(req, handshake.str());
            conn.deliver_websocket(m_pub_key, m_pub_idents, "{\"listeners\":\"ok\"}");
            listeners++;
        }
        else
        {
            continue;
        }
        std::cout << "Players: " << seat << " WebSocket Listeners" << listeners << std::endl;
    }
    m_players[0].m_is_dealer = true;

}

void Smithers::publish_to_all(const std::string& message)
{

    zmq::message_t zmq_message(message.begin(), message.end());
    m_pub_socket.send(zmq_message);
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

void Smithers::play_tournament(int chips, int min_raise, int hands_before_blind_double)
{
    for (size_t i=0; i<m_players.size(); i++)
    {
        m_players[i].m_in_play= true;
        m_players[i].m_in_play_this_round = true;
        m_players[i].m_chips = chips;
        m_players[i].m_chips_this_game = 0;
        m_players[i].m_chips_this_round = 0;

    } 

    int hands_count = 0;
    while ( player_utils::count_active_players(m_players) > 1 )
    {
        if (hands_count % hands_before_blind_double == 0  && hands_count != 0)
        {
            min_raise *= 2;
        }

        GameRunner game(m_players, m_publistener, m_pub_idents, m_pub_key, m_pub_socket);
        game.play_game(min_raise);
        
        player_utils::mark_broke_players(m_players);
        
        hands_count++;
    }

    players_cit_t win_it = std::find_if( m_players.begin(), m_players.end(), [](const Player p){return p.m_chips>0;} );
    publish_to_all( create_tournament_winner_message( win_it->m_name, win_it->m_chips ) );

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


} // smithers namespace