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
        BettingGame(std::vector<Player>& players, 
                    m2pp::connection& pub_list_m2con,
                    const std::vector<std::string>& pub_ids,
                    const std::string& pub_key,
                    zmq::socket_t& pub_socket); 

        void run_pocket_betting_round(int min_raise);
        void run_flop_betting_round(int min_raise);
        void run_turn_betting_round(int min_raise);
        void run_river_betting_round(int min_raise);

        void publish_to_all(const std::string& message);
        void publish_to_all(const Json::Value& json);

        int get_pot_value() const;

    private:

        void run_betting_round(int first_to_play, int min_raise, int last_bet);
        
        Json::Value get_players_move(const Player& player, int min_raise, int last_bet);
        PlayerMove_t process_move(const Json::Value& move, const Player& player, int min_raise, int last_bet) const;
        void do_players_move(Player& player, const PlayerMove_t& move);

        void end_round_betting();

        void do_blinds(int little_blind, int big_blind);

        Json::Value listen_and_pull_from_queue(const std::string& player_name);

        std::vector<Player>& m_players;
        
        // ** sockets **
        m2pp::connection& m_publist;
        zmq::socket_t& m_pub_socket;

        // ** delivery data **     
        const std::vector<std::string>& m_pub_ids;
        const std::string& m_pub_key;


}; 


} // smithers

#endif
