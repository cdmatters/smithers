#ifndef GAME_RUNNER
#define GAME_RUNNER

#include "player.h"
#include "messages.h"

#include "hand_rank.h"

#include <zmq.hpp>
#include <m2pp.hpp>

#include <vector>

namespace smithers{

class GameRunner{
    public:
        GameRunner(std::vector<Player>& players, 
                    m2pp::connection& pub_list_m2con, 
                    std::vector<std::string>& pub_ids,
                    const std::string& pub_key,
                    zmq::socket_t& pub_socket);

        void play_game(int min_raise);

    private:
        
        void reset_and_move_dealer_to_next_player();
        std::vector<Result_t> award_winnings(const std::vector<ScoredFiveCardsPair_t>& scored_hands);
        int assign_seats(int dealer_seat);
               

        std::vector<Player>& m_players;

        // ** sockets **
        m2pp::connection& m_publist;
        zmq::socket_t& m_pub_socket;

        // ** delivery data **
        std::vector<std::string>& m_pub_ids;
        const std::string& m_pub_key;


};

}


#endif 