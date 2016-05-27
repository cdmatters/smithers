#ifndef PLAYER_UTILS
#define PLAYER_UTILS

#include "player.h"

#include <string>
#include <vector>

namespace smithers {
namespace player_utils{

int get_dealer(const std::vector<Player>& players);
int get_next_to_play(const std::vector<Player>& players, int seat);

void mark_broke_players(std::vector<smithers::Player>& players);

int count_active_players(const std::vector<Player>& players);


}
} // close smithers

#endif