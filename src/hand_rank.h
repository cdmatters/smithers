#ifndef HAND_RANK
#define HAND_RANK

#include "card.h"

namespace smithers{

int rank_hand(const std::vector<Card>& table, const Hand& hand);
int rank_five_cards(std::vector<Card>& cards );

}

#endif