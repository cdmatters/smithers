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


    
    class Smithers{

    
    public:
        Smithers();

        void await_registered_players(const int max_players);
        void play_game();
        
        void publish_to_all(const std::string& message);
        void publish_to_all(const Json::Value& json);

        void print_players();

        std::string create_new_game_message();
        Json::Value create_dealt_hands_message(const std::vector<Hand>& hands);
        Json::Value create_table_cards_message(const std::vector<Card>& cards);
        Json::Value create_move_request(const std::string& name);

        enum MessageType {
            NEW_GAME=0,
            DEALT_HANDS, 
            BET_REQUEST,
            BET,
            TABLE_CARDS, 
            RESULTS,
        };
    private:

        void play_betting_round(int first_to_play);

        void listen_and_pull_from_queue();

        std::vector<Player> m_players;

        zmq::context_t m_zmq_context;
        zmq::socket_t m_publisher;



    };




}

#endif 