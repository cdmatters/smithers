from bot_framework import PokerBotFramework

import random

class PokerBot(PokerBotFramework):
    def __init__(self, name, server_url, listening_socket=None ):
        super(PokerBot, self).__init__( name, server_url, listening_socket)
        self.competitors = []
        self.CompetitorModel = dict

    def register(self):
        super(PokerBot, self).register()

    def play(self):
        super(PokerBot, self).play()

    def set_up_competitors(self, players):
        for p in players:
            c = self.CompetitorModel(name=p["name"], chips=p["chips"])
            self.competitors.append(c)


    def receive_tournament_start_message(self, players):
        '''Triggered after all registered players are in, and after ordered dict 
        of competitors is instantiated using the class set via:
            - attribute  self.CompetitorModel  (= object by default)
         or - method     self.load_player_class(competitorClass=object) 
        These competitors are stored as a bot object attribute allowing for updating
        of player model throughout the whole tournament
            - self.competitors = {"PlayerName1":self.CompetitorModel()} etc...
        '''
        print "tournament starting: %s players: %s" % (len(players), ", ".join([p["name"] for p in players]))

    def receive_move_message(self, player_name, move, amount, chips_left, is_blind):
        '''Triggered when a player's moved is published. 
                - Includes own players move and blinds. 
                - Arg `move` is in {"RAISE", "CALL", FOLD", "ALL_IN"}
                args(self, str, str, int, int, bool) 
                        -> eg ("Player1", "RAISE", 1000, 9000, False)
         '''
        print "received a move from another player: %s, move: %s, amount: %s, chips_left: %s, is a blind? %s" % (
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
               - Arg `result` is (player, winnings, hand).
               - results_list is sorted win -> loser
               args(self, list(result))
        '''
        print "received the results of the hand:"
        for r in results_list:
            print "RESULTS: player: %s, winnings: %s, hand: %s" % (r[0], r[1], r[2])

    def on_move_request(self, min_raise, call, pot, current_bet, chips):
        '''Triggered on request for a move from the bot. 
                - RETURN: tuple(move, chips)  
                    `move` in {"RAISE_TO", "CALL", "FOLD"} 
                    `chips` is amount of chips betting
                args(self, int, int, int, int, int)
        '''
        moves = [
            ("RAISE_TO", min_raise),
            ("CALL", call),
            ("FOLD", 0)
        ]

        move = random.choice(moves)
        print "moved: %s, %s" % move
        # TBD can check is valid
        # self.is_valid_move(move) 
        return move


if __name__=="__main__":
    name = raw_input('Enter BOTNAME: ') 
    #name = name if name else "randombot":
    tm = raw_input('Test_mode?: ')
    pb = PokerBot(name, "http://localhost:6767") # "raw_socket_listener -> tcp://127.0.0.1:9950"
    pb.is_test = True if tm else False
    pb.register()
    pb.play()