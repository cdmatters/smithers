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

        self.isTest = False


    def _connect_to_socket(self):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB)
        
        self.socket.connect(self.socket_url)
        self.socket.setsockopt(zmq.SUBSCRIBE, '')


    def _get_message_from_socket(self):
        # TBD. loop and get a timeout
        message = self.socket.recv()
        # TBD. exceptions caught for failures
        json_message = json.loads(message);
        # loop over to check valid "types" in json
        return json_message

    def _send_message_to_server(self, server_url, json_msg):
        # TBD. error checking 
        return requests.post(server_url, json=json_msg)

    def _process_players(self):
        '''Get notification of players and store in internal model, using
        internal model of players'''
        pass

    def _build_move(self, move):
        assert(self.is_valid_move(move))
        return  {
            "name":self.name,
            "move":move[0],
            "chips":int(move[1]),
        }

    def _send_move_to_server(self, move):
        data = self._build_move(move)
        url = self.server_url+"/move/"
        self._send_message_to_server(url, data)


    def load_player_class(self, PlayerClass=object):
        self.OtherPlayerModel = PlayerClass

    @abc.abstractmethod
    def receive_players_message(self):
        """What to be done when list of players is received"""
        return

    @abc.abstractmethod
    def receive_move_message(self, is_blind):
        """What to be done when a move is received"""
        return

    @abc.abstractmethod
    def receive_hands_message(self):
        """What to be done when hands are received"""
        return

    @abc.abstractmethod
    def receive_board_message(self):
        """What to be done when the board is received"""
        return

    @abc.abstractmethod
    def receive_results_message(self):
        """What to be done when the results are received"""
        return

    @abc.abstractmethod
    def on_move_request(self, min_raise, call):
        """What to be done when a move is received"""
        return

    def is_valid_move(self, move):
        return  (len(move)==2
                 and move[0] in ["RAISE", "RAISE_TO", "CALL", "FOLD"]
                 and move[1] >= 0)

    def register(self):
        data = { "name" : self.name }
        # TBD. check for errors
        url  = self.server_url+'/register/'
        r = self._send_message_to_server(url, data)
        print r.json()

    def play(self):
        self._connect_to_socket()
        while True:
            if self.isTest == True:
                raw_input()
            msg = self._get_message_from_socket()
            m_type = msg.get("type", None)
            if m_type == "DEALT_HANDS":
                self.receive_hands_message()
            elif m_type == "DEALT_BOARD":
                self.receive_board_message()
            elif m_type == "BLIND":
                self.receive_move_message(True)
            elif m_type == "MOVE":
                self.receive_move_message(False)
            elif m_type == "RESULTS":
                self.receive_results_message()
            elif m_type == "MOVE_REQUEST":
                if msg.get("name", None) == self.name:
                    move = self.on_move_request(msg["raise"], msg["call"])
                    self._send_move_to_server(move)





if __name__== "__main__":
    name = raw_input('Enter RAW BOTNAME: ')
    pb = PokerBotFramework("http://localhost:6767","tcp://127.0.0.1:9950", name)
    pb.register()
    # pb.play()

