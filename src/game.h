#ifndef GAME 
#define GAME

namespace smithers{

enum Suit { C, D, H, S };
enum Rank { _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _J, _Q, _K, _A};

struct Card{
    enum Suit suit;
    enum Rank rank;  
};

struct Hand{
    const Card card1;
    const Card card2;
    bool in_play;
    bool is_dealer;
};

class Game{
    public:

        const std::vector<Hand>& deal_hand(int number_of_players);
        const std::vector<Card>& deal_flop();
        const Card& deal_river();
        const Card& deal_turn();

        pair<int,Hand> mark_winning_hand(); //signiature?


        const std::vector<Card>& get_table();

    private:
        std::vector<Hand> hands;
        std::vector<Card> burnt;
        std::vector<Card> table;
};

}

# endif