#ifndef GAME 
#define GAME

#include <vector>

#include <json/json.h>

namespace smithers{



struct Card{
    static const char S[5];
    static const char R[14]; 
    int rank;
    int suit;  
};
std::ostream& operator<<(std::ostream& stream, const Card& card);
Json::Value& operator<<(Json::Value& j_val, const Card& card);

struct Hand{
    const Card card1;
    const Card card2;
    bool in_play;
    bool is_dealer;
    int last_bet;
};
std::ostream& operator<<(std::ostream& stream, const Hand& hand);
Json::Value& operator<<(Json::Value& j_val, const Hand& hand);

class Game{
    public:
        Game();

        const std::vector<Hand>& deal_hands(int number_of_players);

        std::vector<Card> deal_flop();
        Card deal_river();
        Card deal_turn();

        const std::vector<Card>& get_table();
        const std::string get_table_str();

        std::pair<int,Hand> mark_winning_hand(); //signiature?
        

    private:
        void build_deck();

        const Card deal_a_card();
        void burn_card(const Card card);

        std::vector<Card> m_deck;
        std::vector<Card> m_burnt;
        std::vector<Card> m_table;
        std::vector<Hand> m_hands;

        int m_pot;
        int last_bet;


};

}

# endif