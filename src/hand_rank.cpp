#include "hand_rank.h"

#include <sstream>
#include <functional>
#include <algorithm>
#include <vector>


namespace smithers{

std::ostream& operator<<(std::ostream& stream, const FiveCards_t& fivecard)
{
    stream << fivecard[0] << " " 
           << fivecard[1] << " " 
           << fivecard[2] << " " 
           << fivecard[3] << " " 
           << fivecard[4]   ;
    return stream;
}


void generate_five_card_combos(const std::vector<Card>& cards, std::vector<FiveCards_t>* output_combos){
    for (int i=0; i<7; i++)
    {
       std::vector<int> chose_1 = {0,1,2,3,4,5,6};
       chose_1.erase(chose_1.begin()+i);
       for (int j=0; j<6; j++)
       {
           std::vector<int> chose_2(chose_1);
           chose_2.erase(chose_2.begin()+j);
           FiveCards_t this_hand{{cards[chose_2[0]],
                                  cards[chose_2[1]],
                                  cards[chose_2[2]],
                                  cards[chose_2[3]],
                                  cards[chose_2[4]]}};
           output_combos->push_back(this_hand);
        }
    }
}

bool card_comparator(const smithers::Card& c1, const smithers::Card& c2)
{
    return (c1.rank <= c2.rank);
}

bool score_pair_comparator(const ScoredFiveCardsPair_t& s1, const ScoredFiveCardsPair_t& s2)
{ 
    return (s1.first > s2.first);
}

bool is_card_this_rank(const smithers::Card& c1, int rank)
{
    return c1.rank == rank;
}

bool is_card_this_suit(const smithers::Card& c1, int suit)
{
    return c1.suit == suit;
}

int count_paired_cards(const FiveCards_t& cards )
{
    int count=0;
    for (int i=0; i<5; ++i)
    {
        if (2 == std::count_if(cards.cbegin(), cards.cend(), 
                               std::bind(           
                                    is_card_this_rank,
                                    std::placeholders::_1,
                                    cards[i].rank) ) )
        {
            count++;
        }
    }
    return count/2;
}

bool is_pair(const FiveCards_t& cards )
{
     return count_paired_cards(cards) ==1;
}

bool is_two_pair(const FiveCards_t& cards )
{
     return count_paired_cards(cards) ==2;
}

bool is_three_of_kind(const FiveCards_t& cards )
{
    return 3 == std::count_if(cards.cbegin(),
                                cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1,
                                    cards[0].rank )
                                ) 
           || 3 == std::count_if(cards.cbegin(),
                                cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1, 
                                    cards[1].rank 
                                    )
                                )
           || 3 == std::count_if(cards.cbegin(),
                                cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1, 
                                    cards[2].rank 
                                    )
                                ) ;
}

bool is_straight(const FiveCards_t& cards )
{
    FiveCards_t::const_iterator c = cards.cbegin() + 1;
    for (;c != cards.cend(); c++) //woo!
    {
        if (c->rank != (c-1)->rank + 1)
        {
            return false;
        }
    }
    return true;
}

bool is_flush(const FiveCards_t& cards )
{
    return std::all_of(cards.cbegin(),cards.cend(),
                            std::bind(
                                is_card_this_suit,
                                std::placeholders::_1,
                                cards[0].suit )
                                ); 
}

bool is_full_house(const FiveCards_t& cards )
{  
    return is_three_of_kind(cards) && is_pair(cards);
}

bool is_straight_flush(const FiveCards_t& cards )
{
    return is_straight(cards) && is_flush(cards);
}

bool is_four_of_kind(const FiveCards_t& cards )
{
    return 4 == std::count_if(cards.cbegin(),cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1,
                                    cards[0].rank )) 
         || 4 == std::count_if(cards.cbegin(),cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1, 
                                    cards[1].rank )
                                ) ;
}

int pick_hand_type(const FiveCards_t& cards)
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

ScoredFiveCardsPair_t create_scored_pair(FiveCards_t& five)
{
    ScoredFiveCardsPair_t p(score_five_cards(five), five);
    return p;
}

ScoredFiveCardsPair_t rank_hand(const std::vector<Card>& table, const Hand& hand)
{
    std::vector<Card> cards;
    for (size_t i=0; i<table.size(); i++)
    {
        cards.push_back(table[i]);
    }
    cards.push_back(hand.card1);
    cards.push_back(hand.card2);

    std::vector<FiveCards_t> all_five_cards;
    generate_five_card_combos(cards, &all_five_cards);

    std::vector<ScoredFiveCardsPair_t> scored_five_card_pairs;
    scored_five_card_pairs.resize(all_five_cards.size());

    std::transform(
        all_five_cards.begin(), all_five_cards.end(),
        scored_five_card_pairs.begin(), create_scored_pair
        );

    std::sort(scored_five_card_pairs.begin(), scored_five_card_pairs.end(), score_pair_comparator);
    return scored_five_card_pairs[0]; //highest

}

int score_five_cards(FiveCards_t& cards )
{
    const char to_hex[] = {"0123456789ABCDEF"};
    
    std::sort(cards.begin(), cards.end(), card_comparator);
    
    std::ostringstream rank;
    for (size_t i=0; i<cards.size(); i++)
    {
        rank << to_hex[cards[i].rank];
    }
    rank << to_hex[pick_hand_type(cards)] << std::endl; 
    std::string rank_str = rank.str();
    std::reverse(rank_str.begin(), rank_str.end());

    return  std::stoul(rank_str, NULL, 16); // hex number
}



}