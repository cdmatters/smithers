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
    if (players[next].m_in_play 
        && players[next].m_in_play_this_round)
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

int count_active_players_in_game(const std::vector<Player>& players)
{
    return std::count_if(players.cbegin(), players.cend(), [](const Player p){return p.m_in_play==true &&
                                                                                p.m_in_play_this_round==true;});
};

int count_not_all_in_active_players_in_game(const std::vector<Player>& players)
{
    return std::count_if(players.cbegin(), players.cend(), [](const Player p){return p.m_in_play==true &&
                                                                                p.m_in_play_this_round==true &&
                                                                                p.m_all_in_this_round==false;});
};

// ONE LINERS -> see std::sum when you get internet again...
int get_pot_value_for_round(const std::vector<Player>& players) 
{
    int sum = 0; 
    for (std::vector<Player>::const_iterator c_it = players.cbegin(); c_it != players.cend(); c_it++)
    {
        sum += c_it->m_chips_this_round;
    }
    return sum;
};
int get_pot_value_for_game(const std::vector<Player>& players) 
{
    int sum = 0; 
    for (std::vector<Player>::const_iterator c_it = players.cbegin(); c_it != players.cend(); c_it++)
    {
        sum += c_it->m_chips_this_game;
        sum += c_it->m_chips_this_round;
    }
    return sum;
};


int get_all_chips_at_table(const std::vector<Player>& players) 
{
    int sum = 0; 
    for (std::vector<Player>::const_iterator c_it = players.cbegin(); c_it != players.cend(); c_it++)
    {
        sum += c_it->m_chips_this_game + c_it->m_chips_this_round + c_it->m_chips;
    }
    return sum;
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

void transfer_round_bets_to_game_bets(std::vector<Player>& players)
{ 
    for (std::vector<Player>::iterator it = players.begin(); it != players.end(); it++)
    {
        it->m_chips_this_game += it->m_chips_this_round; 
        it->m_chips_this_round = 0;

    }
};





} // close player_utils 
} // close smithers