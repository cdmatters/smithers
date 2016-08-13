#include "messages.h"  


namespace smithers{

Json::Value create_registered_message( const Player& player )
{
    Json::Value root;

    root["type"] = "REGISTERED";
    root["name"] = player.m_name;
    root["chips"] = player.m_chips;
    root["key"] = player.m_hash_key;

    return root;
}



Json::Value create_dealt_hands_message(
        const std::vector<Hand>& hands,
        const std::vector<Player>& players,
        int dealer)
{
    int hand_number = 0;
    Json::Value players_json(Json::arrayValue);
    
    for (size_t i=0; i<players.size(); ++i){
        
        int deal_to_seat = (dealer + 1 + i) % players.size(); // start left of dealer

        if (!players[deal_to_seat].m_in_play){
            continue;
        }

        Json::Value player;
        player["name"] = players[deal_to_seat].m_name;
        player["chips"] = players[deal_to_seat].m_chips - players[deal_to_seat].m_chips_this_round - players[deal_to_seat].m_chips_this_game ;
        player["hand"] << hands[hand_number];

        players_json.append(player);
        hand_number++;
    }

    Json::Value root;
    root["type"] = "DEALT_HANDS";
    root["pot"] = 0;
    root["players"] = players_json;

    return root;
}

Json::Value create_tournament_winner_message(const std::string& winner, int chips)
{
    Json::Value root;
    root["type"]="WINNER";
    root["name"]=winner;
    root["winnings"] = chips;

    return root;
}

Json::Value create_table_cards_message(const std::vector<Card>& cards, int pot)
{
    Json::Value card_vector(Json::arrayValue);
    for (std::vector<Card>::const_iterator c_it = cards.cbegin();
        c_it != cards.cend();
        c_it++){
        // ugly
        std::ostringstream c;
        c << *c_it;
        card_vector.append(c.str());
    } 

    Json::Value root;
    root["type"] = "DEALT_HANDS";
    root["pot"] = pot;
    root["cards"] = card_vector;

    return root;
}

Json::Value create_move_request(const Player& player, int pot, int min_raise, int last_bet)
{
    Json::Value root;
    root["type"] = "MOVE_REQUEST";
    root["pot"] = pot;
    root["name"] = player.m_name;
    root["min_raise"] = min_raise;
    root["last_bet"] = last_bet;
    root["chips"] = player.m_chips - player.m_chips_this_round - player.m_chips_this_game;
    return root;
}

Json::Value create_move_message(const Player& player, enum MoveType move, int amount)
{
    std::string move_string;
    switch (move)
    {
        case FOLD:
            move_string="FOLD";
            amount = 0;
            break;
        case RAISE:
            move_string="RAISE_TO";
            break;
        case CALL:
            move_string="CALL";
            break;
        case ALL_IN:
            move_string="ALL_IN";
            break;
        case ERROR: // shouldn't happen
            move_string="ERROR";
            break;
    }

    Json::Value root;
    
    root["type"] = "MOVE";
    root["move"] = move_string;
    root["bet"] = amount;
    root["name"] = player.m_name;
    root["chips"] = player.m_chips - player.m_chips_this_round - player.m_chips_this_game;

    return root;
}

Json::Value create_results_message(const std::vector<Result_t>& results, const std::vector<Player>& players)
{
    Json::Value root;
    root["type"] = "RESULTS";
    Json::Value players_json(Json::arrayValue);
    for (size_t i=0; i<results.size(); i++){
        Json::Value p;
        p["name"] = players[results[i].player_index].m_name;
        p["hand"] = results[i].hand;
        p["winnings"] = results[i].winnings;
        players_json.append(p);
    }
    root["players"] = players_json;
    return root;

};

}