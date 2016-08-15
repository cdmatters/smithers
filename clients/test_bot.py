from bot_framework import PokerBotFramework

class PokerBot(PokerBotFramework):
    def __init__(self, server_url, listening_socket, name):
        super(PokerBot, self).__init__(server_url, listening_socket, name)

    def play(self):
        super(PokerBot, self).__init__(server_url, listening_socket, name)
        

    def receive_move_message(self, name):
        print "name", name



if __name__=="__main__":
    name = raw_input('Enter BOTNAME: ')
    pb = PokerBot("http://localhost:6767","tcp://127.0.0.1:9950", name)
    pb.register()
    pb.play()