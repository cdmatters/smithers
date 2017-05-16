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

std::pair<int, int> get_paired_cards(const FiveCards_t& cards )
{
    std::vector<int> pairs = {};
    for (int i=0; i<5; ++i)
    {
        if (2 == std::count_if(cards.cbegin(), cards.cend(), 
                               std::bind(           
                                    is_card_this_rank,
                                    std::placeholders::_1,
                                    cards[i].rank) ) )
        {
            pairs.push_back(cards[i].rank + 1);
        }
    }
    std::sort(pairs.begin(), pairs.end());


    std::pair<int, int> result = {0,0};

    if (pairs.size() ==4)
    {
        result.first = pairs[0] ;
        result.second = pairs[2];
    }
    else if (pairs.size() ==2)
    {
        result.first = pairs[0];
         
    }

    return result; 
}

int get_three_of_kind(const FiveCards_t& cards )
{
    if (3 == std::count_if(cards.cbegin(),
                                cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1,
                                    cards[0].rank )
                                ))
    {
        return  cards[0].rank + 1;
    } 
    else if (3 == std::count_if(cards.cbegin(),
                                cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1, 
                                    cards[1].rank 
                                    )
                                ))
    {
        return cards[1].rank + 1; 
    }
    else if (3 == std::count_if(cards.cbegin(),
                                cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1, 
                                    cards[2].rank 
                                    )
                                ))
    {
        return cards[2].rank + 1;
    }
    else
    {
        return 0;
    }
}

bool is_straight(const FiveCards_t& cards )
{
    FiveCards_t::const_iterator c = cards.cbegin() + 1;
    for (;c != cards.cend(); c++) //woo!
    {
        if (c->rank != (c-1)->rank + 1) // cards are sorted: check succesive
        {
            // edge case: 2,3,4,5,A 
            bool is_2345A = (c + 1 == cards.cend() // last check
                    && cards.cbegin()->rank == 0    // started with 2
                    && c->rank == 12);              // ended with A
            if (!is_2345A)
            {
                return false;
            }

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
    return (get_three_of_kind(cards) != 0) && (get_paired_cards(cards).first!=0);
}

bool is_straight_flush(const FiveCards_t& cards )
{
    return is_straight(cards) && is_flush(cards);
}

int get_four_of_kind(const FiveCards_t& cards )
{
    if ( 4 == std::count_if(cards.cbegin(),cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1,
                                    cards[0].rank ))
        )
    {
        return cards[0].rank + 1;
    } 
    else if (4 == std::count_if(cards.cbegin(),cards.cend(),
                                std::bind(
                                    is_card_this_rank,
                                    std::placeholders::_1, 
                                    cards[1].rank ) )
        )
    {
        return cards[1].rank + 1;
    }
    else
    {
        return 0;
    }

                                 
}


std::string score_five_cards(FiveCards_t& cards )
{
    const char to_letter[] = {"ABCDEFGHIJKLMN"};
    
    std::sort(cards.begin(), cards.end(), card_comparator);
    
    std::ostringstream rank;
    for (size_t i=0; i<cards.size(); i++)
    {
        rank << to_letter[cards[i].rank];
    }
    std::pair<int, int> pairs = get_paired_cards(cards);
    rank << to_letter[pairs.first] <<  to_letter[pairs.second];
    rank << to_letter[get_three_of_kind(cards)];
    rank << to_letter[is_straight(cards)];
    rank << to_letter[is_flush(cards)];
    rank << to_letter[is_full_house(cards)];
    rank << to_letter[get_four_of_kind(cards)];
    rank << to_letter[is_straight_flush(cards)];

    rank << std::endl; 
    std::string rank_str = rank.str();
    std::reverse(rank_str.begin(), rank_str.end());

    return  rank_str; // hex number
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






}