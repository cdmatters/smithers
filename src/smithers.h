#ifndef SMITHERS
#define SMITHERS

#include "player.h"
#include "game.h"
#include <zmq.hpp>
#include <json/json.h>

#include <string>
#include <vector>



namespace smithers{
    typedef std::vector<Player>::iterator players_it_t;
    typedef std::vector<Player>::const_iterator players_cit_t;


    enum MoveType {
            RAISE,
            CALL,
            FOLD,
            ALL_IN
    };
    
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
        Json::Value create_dealt_hands_message(const std::vector<Hand>& hands);
        Json::Value create_table_cards_message(const std::vector<Card>& cards);
        Json::Value create_move_request(const Player& player, int pot, int last_bet);
        Json::Value create_move_message(const Player& player, enum MoveType move, int amount);

        enum MessageType {
            NEW_GAME=0,
            DEALT_HANDS, 
            BET_REQUEST,
            BET,
            TABLE_CARDS, 
            RESULTS,
        };


    private:

        void play_betting_round(int first_to_play, int min_raise, int last_bet, std::vector<std::string>& side_pots);
        Json::Value listen_and_pull_from_queue(const std::string& player_name);
        enum MoveType process_move(const Json::Value& move, Player&, int& min_raise, int& last_bet);

        std::vector<Player> m_players;

        
        int get_dealer();
        void reset_and_move_dealer_to_next_player();
        int get_next_to_play(int i);
        int get_next_not_broke(int i);
        int get_pot_value_for_game();
        void put_betting_round_in_pot();
        void award_winnings(const std::vector<ScoredFiveCardsPair_t>& scored_hands);


        zmq::context_t m_zmq_context;
        zmq::socket_t m_publisher;



    };




}

#endif 