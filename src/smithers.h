#ifndef SMITHERS
#define SMITHERS

#include "player.h"
#include "messages.h"

#include "hand_rank.h"

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
        void play_game(int min_raise);
        void play_tournament();
        
        void publish_to_all(const std::string& message);
        void publish_to_all(const Json::Value& json);

        void print_players();

        std::string create_new_game_message();

        // Json::Value create_results_message(const std::vector<Result_t>& results);

    private:

        Json::Value listen_and_pull_from_queue(const std::string& player_name);


        int assign_seats(int dealer_seat);
        void reset_and_move_dealer_to_next_player();
        int get_pot_value_for_game();

        std::vector<Result_t> award_winnings(const std::vector<ScoredFiveCardsPair_t>& scored_hands);


        std::vector<Player> m_players;
        zmq::context_t m_zmq_context;
        zmq::socket_t m_publisher;



    };




}

#endif 