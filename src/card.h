#ifndef CARD
#define CARD


#include <iostream>
#include <sstream>
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
};
std::ostream& operator<<(std::ostream& stream, const Hand& hand);
Json::Value& operator<<(Json::Value& j_val, const Hand& hand);


}

#endif