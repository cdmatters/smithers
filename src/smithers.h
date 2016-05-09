#ifndef SMITHERS
#define SMITHERS

#include "player.h"
#include <zmq.hpp>

#include <string>
#include <vector>


namespace smithers{
    typedef std::vector<Player>::iterator players_it_t;
    typedef std::vector<Player>::const_iterator players_cit_t;

    
    class Smithers{
    
    public:
        Smithers();

        void await_registered_players(const int max_players);
        void play_game();
        void publish_to_all(const std::string& json) ;

        void print_players();


    private:

        std::vector<Player> m_players;
        std::vector<int> m_subscribers;

        zmq::context_t m_zmq_context;
        zmq::socket_t m_publisher;



    };




}

#endif 