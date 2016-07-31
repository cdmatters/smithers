#ifndef BETTINGGAME
#define BETTINGGAME

#include "player.h"

#include <zmq.hpp>
#include <m2pp.hpp>
#include <json/json.h>

#include <vector>

namespace smithers{


class BettingGame{
    public:
        BettingGame(m2pp::connection& m_listener, zmq::socket_t& m_publisher, std::vector<Player>& m_players);

        void run_pocket_betting_round();
        void run_flop_betting_round();
        void run_river_betting_round();
        void run_turn_betting_round();

        void publish_to_all(const std::string& message);
        void publish_to_all(const Json::Value& json);
    private:

        void play_betting_round(int first_to_play, int min_raise, int last_bet);
        
        m2pp::connection& m_listener;
        zmq::socket_t& m_publisher;
        std::vector<Player>& m_players;


}; 


} // smithers

#endif
