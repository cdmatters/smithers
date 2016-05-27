 #include "player_utils.h"

#include <iostream>

namespace smithers{

namespace player_utils{

int get_dealer(const std::vector<Player>& players)
{
    std::vector<Player>::const_iterator cit = std::find_if(players.cbegin(), players.cend(), [](const Player& p){return p.m_is_dealer;});
    return cit - players.cbegin(); 
};

int get_next_to_play(const std::vector<Player>& players, int seat)
{
    int next = (seat + 1) % players.size();
    if (players[next].m_in_play && players[next].m_in_play_this_round)
    {
        return next;
    }
    else 
    {
        return get_next_to_play(players, next);
    }
};

int count_active_players(const std::vector<Player>& players)
{
    return std::count_if(players.cbegin(), players.cend(), [](const Player p){return p.m_chips>0;});
};


void mark_broke_players(std::vector<smithers::Player>& players)
{
    for (size_t i=0; i<players.size(); i++)
    {
        if (players[i].m_chips<=0)
        {
            players[i].m_in_play = false;
        };
    }
};

} // close player_utils 
} // close smithers