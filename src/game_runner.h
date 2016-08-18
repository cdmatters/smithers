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
        GameRunner(std::vector<Player>& players, m2pp::connection& listener, zmq::socket_t& publisher);

        void play_game(int min_raise);

    private:
        
        void reset_and_move_dealer_to_next_player();
        std::vector<Result_t> award_winnings(const std::vector<ScoredFiveCardsPair_t>& scored_hands);
        int assign_seats(int dealer_seat);
               

        // m2pp::connection& m_conn;
        std::vector<Player>& m_players;
        m2pp::connection& m_listener;
        zmq::socket_t& m_publisher;


};

}


#endif 