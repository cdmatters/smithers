#ifndef SMITHERS
#define SMITHERS

#include "player.h"
#include "messages.h"

#include "hand_rank.h"

#include <zmq.hpp>
#include <m2pp.hpp>

#include <json/json.h>

#include <string>
#include <vector>



namespace smithers{
    typedef std::vector<Player>::iterator players_it_t;
    typedef std::vector<Player>::const_iterator players_cit_t;

    
class Smithers{
    
    public:
        Smithers();

        void await_registered_players(int max_seats, int min_listeners);
        
        void play_tournament(int chips, int min_raise, int hands_before_blind_double);
        
        void publish_to_all(const std::string& message);
        void publish_to_all(const Json::Value& json);

        void print_players();



    private:
        void reset_players_for_tournament(int chips);
        void mark_broke_players(std::vector<std::string>& broke_player_names);
        void refresh_players_ws(int players);

        void collect_pongs();


        Json::Value listen_and_pull_from_queue(const std::string& player_name);


        std::vector<Player> m_players;

        m2pp::connection m_publistener;

        std::vector<std::string> m_pub_idents;
        std::string m_pub_key;

        zmq::context_t m_zmq_context;
        zmq::socket_t m_pub_socket;




    };




}

#endif 