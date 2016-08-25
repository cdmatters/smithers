from smithers_framework import BotFramework

import random
from collections import OrderedDict

class PokerBot(BotFramework):

    def __init__(self, name, server_url, listening_socket=None):
        super(PokerBot, self).__init__(name, server_url, listening_socket)
        self.competitors = OrderedDict()
        self.CompetitorModel = dict  # insert your own class here

    def register(self):
        super(PokerBot, self).register()

    def play(self):
        super(PokerBot, self).play()

    def set_up_competitors(self, competitors):
        '''TRIGGERED ONCE - on first time list of competitors arrives, at first 
        tournament message. 
            args(list(dict)) - dict.keys() = ["name", "chips"] 

        '''
        for i, c in enumerate(competitors):
            comp = self.CompetitorModel(name=c["name"], chips=c["chips"], seat=i)
            self.competitors[c["name"]] = comp

    def receive_tournament_start_message(self, players):
        '''Triggered on starting new tournament. Players are same format as received
        competitors, but self is included.
                args(list(dict)) - dict.keys() = ["name", "chips"] 
        '''
        print "tournament starting: %s players: %s" % (
                  len(players), ", ".join([p["name"] for p in players]))

    def receive_move_message(self, player_name, move, amount, chips_left, is_blind):
        '''Triggered when a player's moved is published. 
                args(self, str, str, int, int, bool) 
                        -> eg ("Player1", "RAISE", 1000, 9000, False)
                - Does not includes own players move. Only blinds. 
                - Arg `move` is in {"RAISE", "CALL", FOLD", "ALL_IN"}
        '''
        print "received a move from another " + \
              "player: %s, move: %s, amount: %s, chips_left: %s, is a blind? %s" % (
                  player_name, move, amount, chips_left, is_blind)

    def receive_hands_message(self, card1, card2):
        '''Triggered when a players hands are dealt out to the player
                args(self, str, str) -> eg ("4S", "TD")
        '''
        print "received our hands:  %s, %s" % (card1, card2)

    def receive_board_message(self, board, pot):
        '''Triggered when communal cards dealt out and published to player
                args(self, list(str), int) -> eg (["5S", "KH", "JS"], 590) 
        '''
        print "received the board: Board: %s  Pot: %s" % (board, pot)

    def receive_results_message(self, results_list):
        '''Triggered when results of the hand published to player
                args(self, list(result))
                - Arg `result` is (player, winnings, hand).
                - results_list is sorted win -> loser
        '''
        print "received the results of the hand:"
        for r in results_list:
            print "RESULTS: player: %s, winnings: %s, hand: %s" % (r[0], r[1], r[2])

    def receive_broke_message(self, names):
        '''Triggered when a player(s) goes broke/bust.
                args(list(str)) -> eg (["Player1", "Player2"])
        '''
        print "player(s): %s went bust" % (", ".join(names))

    def receive_tournament_winner_message(self, name):
        '''Trigered when a player wins a winner-take-all tournament
                args(str) -> eg "Player1"
        '''
        print "player: %s won the tournament" % name

    def on_move_request(self, min_raise, call, pot, current_bet, chips):
        '''WARNING: Validity of moves is up to you. Smithers will correct any 
        errors with the interest of continuing play. Any mismatch will be printed 
        upon discrepancy.

        Triggered on request for a move from the bot. 
                args(self, int, int, int, int, int)
                - RETURN: tuple(move, chips)  
                    `move` in {"RAISE_TO", "CALL", "FOLD"} 
                    `chips` is amount of chips betting
        '''
        moves = [
            ("RAISE_TO", min_raise),
            ("CALL", call),
            ("FOLD", 0)
        ]
        move = random.choice(moves)
        print "moved: %s, %s" % move
        return move


if __name__ == "__main__":
    import sys
    name = sys.argv[1] if len(sys.argv) >= 2 else ""
    # name = name if name else "randombot":
    # "raw_socket_listener -> tcp://127.0.0.1:9950"
    pb = PokerBot(name, "http://localhost:6767")
    #pb.is_test = True
    pb.register()
    pb.play()
