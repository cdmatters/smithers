#ifndef PLAYER
#define PLAYER

#include <string>

namespace smithers{
    struct Player{
        Player(const std::string& name, const std::string& hash_key, const int seat);
        const std::string m_name;
        const std::string m_hash_key;
        const int m_seat; 
        int m_chips;
        bool m_in_play;
    };



inline Player::Player(const std::string& name, const std::string& hash_key, const int seat)
    : m_name(name), m_hash_key(hash_key), m_seat(seat){};

}
#endif 