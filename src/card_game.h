#ifndef CARDGAME 
#define CARDGAME

#include <vector>

#include <json/json.h>

#include "card.h"
#include "hand_rank.h"

namespace smithers{

class CardGame{
    public:
        CardGame();

        const std::vector<Hand>& deal_hands(int number_of_players);

        std::vector<Card> deal_flop();
        Card deal_river();
        Card deal_turn();

        const std::vector<Card>& get_table();
        const std::string get_table_str();

        std::vector<ScoredFiveCardsPair_t> return_hand_scores();

        

    private:
        void build_deck();

        const Card deal_a_card();
        void burn_card(const Card card);

        std::vector<Card> m_deck;
        std::vector<Card> m_burnt;
        std::vector<Card> m_table;
        std::vector<Hand> m_hands;

};

}

# endif