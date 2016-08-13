#ifndef BETTINGGAME
#define BETTINGGAME

#include "player.h"

#include "messages.h"

#include <zmq.hpp>
#include <m2pp.hpp>
#include <json/json.h>

#include <vector>

namespace smithers{

typedef struct PlayerMove{
    enum MoveType move_type;
    int amount;
    int last_bet;
    int new_min_raise;
} PlayerMove_t;


class BettingGame{
    public:
        BettingGame(m2pp::connection& m_listener, zmq::socket_t& m_publisher, std::vector<Player>& m_players);

        void run_pocket_betting_round();
        void run_flop_betting_round();
        void run_turn_betting_round();
        void run_river_betting_round();

        void publish_to_all(const std::string& message);
        void publish_to_all(const Json::Value& json);

        int get_pot_value() const;

    private:

        void run_betting_round(int first_to_play, int min_raise, int last_bet);
        
        Json::Value get_players_move(const Player& player, int min_raise, int last_bet);
        PlayerMove_t process_move(const Json::Value& move, const Player& player, int min_raise, int last_bet) const;
        void do_players_move(Player& player, const PlayerMove_t& move);

        void end_round_betting();


        Json::Value listen_and_pull_from_queue(const std::string& player_name);

        m2pp::connection& m_listener;
        zmq::socket_t& m_publisher;
        std::vector<Player>& m_players;


}; 


} // smithers

#endif
