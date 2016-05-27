#ifndef SMITHERS
#define SMITHERS

#include "player.h"
#include "game.h"
#include "messages.h"

#include <zmq.hpp>
#include <json/json.h>

#include <string>
#include <vector>



namespace smithers{
    typedef std::vector<Player>::iterator players_it_t;
    typedef std::vector<Player>::const_iterator players_cit_t;

    
class Smithers{
    
    public:
        Smithers();

        void await_registered_players(int max_players, int max_chips );
        void play_game();
        void play_tournament();
        
        void publish_to_all(const std::string& message);
        void publish_to_all(const Json::Value& json);

        void print_players();

        std::string create_new_game_message();

        // Json::Value create_results_message(const std::vector<Result_t>& results);

    private:

        void play_betting_round(int first_to_play, int min_raise, int last_bet);
        Json::Value listen_and_pull_from_queue(const std::string& player_name);
        enum MoveType process_move(const Json::Value& move, Player&, int& min_raise, int& last_bet);

        std::vector<Player> m_players;

        int assign_seats(int dealer_seat);
        void reset_and_move_dealer_to_next_player();
        int get_pot_value_for_game();
        void transfer_round_bets_to_game_bets();
        std::vector<Result_t> award_winnings(const std::vector<ScoredFiveCardsPair_t>& scored_hands);


        zmq::context_t m_zmq_context;
        zmq::socket_t m_publisher;



    };




}

#endif 