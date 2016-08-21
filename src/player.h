#ifndef PLAYER
#define PLAYER

#include <string>

namespace smithers{
    struct Player{
        Player(const std::string& name, const std::string& hash_key, int seat);
        const std::string m_name;
        const std::string m_hash_key;
        int m_seat; 
        int m_chips;
        int m_chips_this_game;
        int m_chips_this_round;
        bool m_in_play;
        bool m_in_play_this_round;
        bool m_all_in_this_round;
        bool m_is_dealer;
    };



inline Player::Player(const std::string& name, const std::string& hash_key, const int seat):
    m_name(name), 
    m_hash_key(hash_key),
    m_seat(seat),
    m_chips(0),
    m_chips_this_game(0),
    m_chips_this_round(0),
    m_in_play(true),
    m_in_play_this_round(true),
    m_all_in_this_round(false),
    m_is_dealer(false)
    {};

}
#endif 