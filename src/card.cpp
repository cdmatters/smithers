#include "card.h"

namespace smithers{

const char Card::S[] = {"CDHS"};
const char Card::R[] = {"23456789TJQKA"};

std::ostream& operator<<(std::ostream& stream, const Card& card)
{
    stream << Card::R[card.rank] << Card::S[card.suit];
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Hand& hand)
{
    stream << "|"  << hand.card1 << " " << hand.card2 << "|" ;
    return stream;
}

Json::Value& operator<<(Json::Value& j_val, const Hand& hand)
{
    std::ostringstream _;
    _ << hand;
    j_val = _.str();
    return j_val;
};

Json::Value& operator<<(Json::Value& j_val, const Card& card)
{
    std::ostringstream _;
    _ << card;
    j_val = _.str();
    return j_val;
};

}
