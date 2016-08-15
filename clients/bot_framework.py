import zmq 
import requests
import json
import abc


class PokerBotFramework(object):
    """ This class will handle all the mechanics for communicating 
    with the server. It will be inherited by any bots that wish to play
    with the server"""
    __metaclass__ = abc.ABCMeta

    def __init__(self, server_url, listening_socket, name):
        self.server_url = server_url
        self.socket_url = listening_socket
        self.socket = None
        self.context = None
        self.name = name 

        self.other_players = []
        self.OtherPlayerModel = object


    def _connect_to_socket(self):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB)
        
        self.socket.connect(self.socket_url)
        self.socket.setsockopt(zmq.SUBSCRIBE, '')


    def get_message_from_socket(self):
        # TBD. loop and get a timeout
        message = self.socket.recv()
        # TBD. exceptions caught for failures
        json_message = json.loads(message);
        # loop over to check valid "types" in json
        return json_message

    def _send_mesage_to_server(self, json_msg):
        # TBD. error checking 
         r = requests.post(server_url, json_msg)

            
    def register(self):
        data = { "name" : self.name }
        # TBD. check for errors
        r = requests.post( self.server_url+'/register/', json=data )
        print r.json()

    def load_player_class(self, PlayerClass=object):
        self.OtherPlayerModel = PlayerClass

    def process_players(self):
        '''Get notification of players and store in internal model, using
        internal model of players'''
        pass

    @abc.abstractmethod
    def receive_move_message(self, name):
        """What to be done when a move is received"""
        return

    def play(self):
        self._connect_to_socket()
        while True:
            msg = self.get_message_from_socket()
            self.receive_move_message(msg)


if __name__== "__main__":
    name = raw_input('Enter RAW BOTNAME: ')
    pb = PokerBotFramework("http://localhost:6767","tcp://127.0.0.1:9950", name)
    pb.register()
    # pb.play()

