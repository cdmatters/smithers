#include "betting_game.h"

#include "player_utils.h"




namespace smithers{

BettingGame::BettingGame(m2pp::connection& listener, zmq::socket_t& publisher, std::vector<Player>& players)
    : m_listener(listener),
    m_publisher(publisher),
    m_players(players)
{


}


void BettingGame::run_betting_round()
{
    
}


void BettingGame::publish_to_all(const std::string& message)
{

    zmq::message_t zmq_message(message.begin(), message.end());
    m_publisher.send(zmq_message);
}

void BettingGame::publish_to_all(const Json::Value& json)
{
    std::ostringstream message;
    message << json;
    publish_to_all(message.str());
}

}