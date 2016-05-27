 #include "player_utils.h"

#include <iostream>

namespace smithers{

namespace player_utils{

int get_dealer(const std::vector<Player>& players)
{
    std::vector<Player>::const_iterator cit = std::find_if(players.cbegin(), players.cend(), [](const Player& p){return p.m_is_dealer;});
    return cit - players.cbegin(); 
};

int get_next_to_play(const std::vector<Player>& players, int i)
{
    return 0;
};

int get_next_not_broke(const std::vector<Player>& players, int i)
{
    return 0;
};

int count_active_players(const std::vector<Player>& players)
{
    return 0;
};

} // close player_utils 
} // close smithers