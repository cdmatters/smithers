from bot_framework import PokerBotFramework

import random

class PokerBot(PokerBotFramework):
    def __init__(self, server_url, listening_socket, name):
        super(PokerBot, self).__init__(server_url, listening_socket, name)

    def play(self):
        super(PokerBot, self).play()



    def receive_players_message(self):
        print "received list of players"

    def receive_move_message(self, player_name, move, amount, chips_left, is_blind):
        print "received a move from another player"
        print "MOVE:\tplayer: %s, move: %s, amount: %s, chips_left: %s, is a blind? %s" % (
                        player_name, move, amount, chips_left, is_blind)

    def receive_hands_message(self, card1, card2):
        print "received our hands:  %s, %s" % (card1, card2)

    def receive_board_message(self, board, pot):
        print "received the board: Board: %s  Pot: %s" %(board, pot)

    def receive_results_message(self, results_list):
        print "received the results of the hand:"
        for r in results_list:
            print "\tplayer: %s, winnings: %s, hand: %s" % (r[0], r[1], r[2])


    def on_move_request(self, min_raise, call):
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
    tm = raw_input('Test_mode?: ')
    pb = PokerBot("http://localhost:6767","tcp://127.0.0.1:9950", name)
    pb.register()
    pb.is_test = True if tm else False
    pb.play()