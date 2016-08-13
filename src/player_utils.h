#ifndef PLAYER_UTILS
#define PLAYER_UTILS

#include "player.h"

#include <string>
#include <vector>

namespace smithers {
namespace player_utils{

int get_dealer(const std::vector<Player>& players);
int get_next_to_play(const std::vector<Player>& players, int seat);

int count_active_players(const std::vector<Player>& players);
int count_active_players_in_game(const std::vector<Player>& players);

int get_pot_value_for_round(const std::vector<Player>& players);
int get_pot_value_for_game(const std::vector<Player>& players); 
int get_all_chips_at_table(const std::vector<Player>& players);

void mark_broke_players(std::vector<smithers::Player>& players);

void transfer_round_bets_to_game_bets(std::vector<smithers::Player>& players); 
}
} // close smithers

#endif