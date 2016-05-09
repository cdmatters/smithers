#ifndef SMITHERS
#define SMITHERS

#include "player.h"

#include <string>
#include <vector>


namespace smithers{
    
    class Smithers{
    
    public:

        void await_registered_players(const int max_players);
        void play_game();
        void publish_to_all(const std::string& json) const;

        void print_players();

    private:

        std::vector<Player> m_players;
        std::vector<int> m_subscribers;

    };


}

#endif 