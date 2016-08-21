#include "game_runner.h"

#include "card_game.h"
#include "betting_game.h"

#include "player_utils.h"



namespace smithers{

bool result_comparator(const Result_t& r1,const Result_t& r2 ){return r1.score>r2.score;}

GameRunner::GameRunner(std::vector<Player>& players, 
                        m2pp::connection& pub_list_m2con, 
                        std::vector<std::string>& pub_ids,
                        const std::string& pub_key,
                        zmq::socket_t& pub_socket)
:m_players(players), m_publist(pub_list_m2con), m_pub_socket(pub_socket), m_pub_ids(pub_ids), m_pub_key(pub_key)
{}


void GameRunner::play_game(int min_raise)
{   
    BettingGame betting_game(m_players, m_publist, m_pub_ids, m_pub_key, m_pub_socket); 
    CardGame card_game;
    
    int dealer_seat = player_utils::get_dealer(m_players);
    assign_seats(dealer_seat);

    std::vector<Hand> hands = card_game.deal_hands( player_utils::count_active_players(m_players) ); 
    
    betting_game.publish_to_all( create_dealt_hands_message( hands, m_players, dealer_seat ) );

    betting_game.run_pocket_betting_round(min_raise);
    
    card_game.deal_flop();
    betting_game.publish_to_all( create_table_cards_message(card_game.get_table(), player_utils::get_pot_value_for_game(m_players) ) );
    betting_game.run_flop_betting_round(min_raise);

    card_game.deal_river();
    betting_game.publish_to_all( create_table_cards_message(card_game.get_table(), player_utils::get_pot_value_for_game(m_players) ) );
    betting_game.run_turn_betting_round(min_raise);
    
    card_game.deal_turn();
    betting_game.publish_to_all( create_table_cards_message(card_game.get_table(), player_utils::get_pot_value_for_game(m_players) ) );
    betting_game.run_river_betting_round(min_raise);

    std::vector<Result_t> results = award_winnings( card_game.return_hand_scores() );
    betting_game.publish_to_all( create_results_message(results, m_players) );

    reset_and_move_dealer_to_next_player();
}


std::vector<Result_t> GameRunner::award_winnings(const std::vector<ScoredFiveCardsPair_t>& scored_hands)
{
    std::vector<Result_t> results;

    for (size_t i=0; i<m_players.size(); i++){   
        
        if (!m_players[i].m_in_play || !m_players[i].m_in_play_this_round)
        {
            continue;
        }
        int seat = m_players[i].m_seat;
        std::ostringstream cards;
        cards <<scored_hands[seat].second;
        Result_t r = {scored_hands[seat].first,cards.str(), i, 0};
        results.push_back(r);
    }

    std::sort(results.begin(), results.end(), result_comparator);

    for (size_t r=0; r<results.size(); r++)
    {
        Player& winner = m_players[results[r].player_index];
        int winners_bet = winner.m_chips_this_game;

        for (size_t p=0; p<m_players.size(); p++){
            int amount = (m_players[p].m_chips_this_game >= winners_bet) ? 
                            winners_bet : m_players[p].m_chips_this_game;
            results[r].winnings += amount;
            m_players[p].m_chips_this_game -= amount;
            m_players[p].m_chips -= amount;
        }
        
        winner.m_chips += results[r].winnings;
    }
        
    return results;
}

int GameRunner::assign_seats(int dealer)
{
    int seat = 0;
    for (size_t i=0; i<m_players.size(); ++i)
    {
        int seat_no = (dealer + i + 1) % m_players.size();
        if ( ! m_players[seat_no].m_in_play )
        {
            m_players[seat_no].m_seat = -1;
        }
        else
        {
            m_players[seat_no].m_seat = seat;
            seat++;
        }
    }
    return seat; // number of players
}

void GameRunner::reset_and_move_dealer_to_next_player()
{
    int dealer = player_utils::get_dealer(m_players);
    for (size_t i=0; i<m_players.size(); ++i){
        m_players[i].m_in_play_this_round = true;
        m_players[i].m_all_in_this_round = false;
    }
    int next_dealer = player_utils::get_next_to_play(m_players, dealer);
    std::cout<< dealer<< " "<< next_dealer << std::endl;

    m_players[dealer].m_is_dealer = false;
    m_players[next_dealer].m_is_dealer = true;
};

}