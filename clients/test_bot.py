from bot_framework import PokerBotFramework

import random

class PokerBot(PokerBotFramework):
    def __init__(self, server_url, listening_socket, name):
        super(PokerBot, self).__init__(server_url, listening_socket, name)

    def play(self):
        super(PokerBot, self).play()



    def receive_players_message(self):
        print "received list of players"

    def receive_move_message(self, is_blind):
        print "received a move from another player"

    def receive_hands_message(self):
        print "received our hands"

    def receive_board_message(self):
        print "received the board"

    def receive_results_message(self):
        print "received the results of the hand"


    def on_move_request(self, min_raise, call):
        print "moved"
        moves = [
            ("RAISE_TO", min_raise),
            ("CALL", call),
            ("FOLD", 0)
        ]

        move = random.choice(moves)
        # TBD can check is valid
        # self.is_valid_move(move) 
        return move



if __name__=="__main__":
    name = raw_input('Enter BOTNAME: ')
    pb = PokerBot("http://localhost:6767","tcp://127.0.0.1:9950", name)
    pb.register()
    # pb.isTest = True
    pb.play()