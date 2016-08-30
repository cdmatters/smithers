#include "card_game.h"

#include <random>
#include <iostream>
#include <sstream>
#include <algorithm>



namespace {

} // close anon


namespace smithers{

CardGame::CardGame()
{
    build_deck();
}


const std::vector<Hand>& CardGame::deal_hands(int number_of_players )
{
    std::vector<Card> all_cards;
    for (int i=0; i< 2*number_of_players; i++){
        all_cards.push_back(deal_a_card());
    }

    for (int i=0; i<number_of_players; i++){
        Hand this_hand = {all_cards[i], all_cards[i+number_of_players]};
        std::cout << this_hand << std::endl;
        m_hands.push_back(this_hand);
    }
    return m_hands;    
}

std::vector<Card> CardGame::deal_flop()
{
    std::vector<Card> flop;
    burn_card(deal_a_card()); // burn 
    for (int i=0; i<3; i++){ // and turn 
        Card c = deal_a_card();
        m_table.push_back(c);
        flop.push_back(c);
    }
    return flop;
}

Card CardGame::deal_river()
{

    burn_card(deal_a_card()); // burn 
    Card c = deal_a_card(); // and turn 
    m_table.push_back(c);

    return c;
}
Card CardGame::deal_turn()
{
    return deal_river(); // code duplication is evil
}

const std::vector<Card>& CardGame::get_table()
{
    return m_table;
};

const std::string CardGame::get_table_str()
{
    std::ostringstream t;
    t << "| ";
    for (size_t i=0; i<m_table.size(); i++){
        t << m_table[i] << " ";
    }
    t << "|";
    return t.str();
}

void CardGame::burn_card(const Card c)
{
    m_burnt.push_back(c);
}

void CardGame::build_deck()
{
    for (int i=0; i<52; ++i){
        const Card card = {(int) i/4, i % 4 }; 
        m_deck.push_back(card); 
    }
}

const Card CardGame::deal_a_card()
{
    std::random_device gen;
    std::uniform_int_distribution<> dis(0, m_deck.size()-1);

    int card_no = dis(gen);

    const Card this_card(m_deck[card_no]);
    m_deck.erase(m_deck.cbegin() + card_no);

    return this_card;
}


std::vector<ScoredFiveCardsPair_t> CardGame::return_hand_scores()
{
    // std::vector<ScoredFiveCardsPair_t> print_this;
    std::vector<ScoredFiveCardsPair_t> hand_scores;
    for (size_t i=0; i<m_hands.size(); i++)
    {
        ScoredFiveCardsPair_t scored_hand = rank_hand(m_table, m_hands[i]);
        hand_scores.push_back(scored_hand);
        // print_this.push_back(scored_hand);
        
    }

    // std::sort(print_this.begin(), print_this.end(),
    //  [](ScoredFiveCardsPair_t a, ScoredFiveCardsPair_t b){return a.first>b.first;});
    // for (size_t j=0; j<print_this.size(); j++)
    // {
    //     std::cout << print_this[j].first << " ";
    //     for (size_t k=0; k<5; k++)
    //     {
    //         std::cout << print_this[j].second[k]<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    return hand_scores;
   
}




} // close smithers