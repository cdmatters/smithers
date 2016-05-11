#ifndef HAND_RANK
#define HAND_RANK

#include "card.h"

#include <array>

namespace smithers{
typedef  std::array<Card,5> FiveCards_t;
typedef  std::pair<int,FiveCards_t> ScoredFiveCardsPair_t;


ScoredFiveCardsPair_t rank_hand(const std::vector<Card>& table, const Hand& hand);
int score_five_cards(FiveCards_t& cards);

}

#endif