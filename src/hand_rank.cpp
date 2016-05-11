#include "hand_rank.h"

#include <sstream>

namespace smithers{

bool card_comparator(const smithers::Card& c1, const smithers::Card& c2)
{
    return (c1.rank <= c2.rank);
}

bool is_pair(const std::vector<Card>& cards )
{

}

bool is_two_pair(const std::vector<Card>& cards )
{

}

bool is_three_of_kind(const std::vector<Card>& cards )
{

}

bool is_straight(const std::vector<Card>& cards )
{

}

bool is_flush(const std::vector<Card>& cards )
{
   
}

bool is_full_house(const std::vector<Card>& cards )
{  
    return is_three_of_kind(cards) && is_pair(cards);
}

bool is_straight_flush(const std::vector<Card>& cards )
{
    return is_straight(cards) && is_flush(cards);
}

bool is_four_of_kind(const std::vector<Card>& cards )
{

}

int pick_hand_type(const std::vector<Card>& cards)
{
    if (is_four_of_kind(cards))
        return 9;
    else if (is_straight_flush(cards))
        return 8;
    else if (is_full_house(cards))
        return 7;
    else if (is_flush(cards))
        return 6;
    else if (is_straight(cards))
        return 5;
    else if (is_three_of_kind (cards))
        return 4;
    else if (is_two_pair (cards))
        return 3;
    else if (is_pair (cards))
        return 2;
    else
        return 1;
}


int rank_hand(const std::vector<Card>& table, const Hand& hand)
{
    std::vector<Card> cards;
    for (size_t i=0; i<table.size(); i++)
    {
        cards.push_back(table[i]);
    }
    // cards.push_back(hand.card1);
    // cards.push_back(hand.card2);

    // combination of 5 cards
    return rank_five_cards(cards); 
    //pick best

}


int rank_five_cards(std::vector<Card>& cards )
{
    // check whether five cards!!
    const char to_hex[] = {"0123456789ABCDEF"};
    
    std::sort(cards.begin(), cards.end(), card_comparator);
    
    std::ostringstream rank;
    for (size_t i=0; i<cards.size(); i++)
    {
        rank << to_hex[cards[i].rank];
    }
    rank << to_hex[pick_hand_type(cards)];

    std::cout<<rank.str() <<std::endl;
    /// reverse and turn to hex
    return 0;
}



}