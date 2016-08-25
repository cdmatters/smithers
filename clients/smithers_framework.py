import zmq
import requests
import json
import abc

from websocket import create_connection


class BotFramework(object):
    """ This class will handle all the mechanics for communicating 
    with the server. It will be inherited by any bots that wish to play
    with the server"""
    __metaclass__ = abc.ABCMeta

    def __init__(self, name, server_url, listening_socket=None):
        self.server_url = server_url
        self.raw_socket_url = listening_socket
        self.socket = None
        self.context = None
        self.name = name

        self.competitors = {}
        self.CompetitorModel = dict

        self.hands_key = None

        self.is_test = False
        self.is_debug = False
        self.use_web_socket = (listening_socket is None)

        self._last_move = None
        self._is_bust = False

    def _connect_to_socket(self):
        if self.use_web_socket:
            ws_server_url = self.server_url.replace(
                "http", "ws", 1) + "/watch/"
            print ws_server_url
            self.socket = create_connection(ws_server_url, timeout=9999999)
        else:
            self.context = zmq.Context()
            self.socket = self.context.socket(zmq.SUB)

            self.socket.connect(self.raw_socket_url)
            self.socket.setsockopt(zmq.SUBSCRIBE, '')

    def _get_message_from_socket(self):
        # TBD. loop and get a timeout
        message = self.socket.recv()
        # TBD. exceptions caught for failures
        json_message = json.loads(message)
        # loop over to check valid "types" in json
        return json_message

    def _send_message_to_server(self, server_url, json_msg):
        # TBD. error checking
        return requests.post(server_url, json=json_msg)

    def _extract_tournament_start(self, tournament_start_msg):
        return tournament_start_msg["players"]

    def _extract_hand(self, dealt_hands_msg):
        players = dealt_hands_msg["players"]

        is_broke = True
        for p in players:
            if p["name"] == self.name:
                cards_string = p["hand"]
                chips = p["chips"]
                is_broke = False

        if is_broke:
            return None

        cards_list = [s for s in cards_string.split("|") if s != ""]
        card1, card2 = cards_list[0].split()
        # TBD: dealer? blind? Other string info?
        return (card1, card2)

    def _extract_board(self, board_hands_msg):
        return (board_hands_msg["cards"], board_hands_msg["pot"])

    def _extract_move(self, move_msg):
        return (move_msg["name"], move_msg["move"], move_msg["bet"], move_msg["chips"])

    def _extract_results(self, results_msg):
        players = results_msg["players"]
        sorted_results = sorted(players, key=lambda p: p[
                                "winnings"], reverse=True)
        return [(p["name"], p["winnings"], p["hand"].split(" ")) for p in sorted_results]

    def _extract_move_request(self, move_request_msg):
        return (move_request_msg["raise"], move_request_msg["call"],
                move_request_msg["current_bet"], move_request_msg["chips"],
                move_request_msg["raise"])

    def _extract_broke(self, broke_msg):
        return broke_msg["names"]

    def _extract_winner(self, winner_msg):
        return winner_msg["name"]

    def _build_move(self, move):
        assert(self._is_valid_move_type(move))
        return {
            "name": self.name,
            "move": move[0],
            "chips": int(move[1]),
        }

    def _send_move_to_server(self, move):
        data = self._build_move(move)
        url = self.server_url + "/move/"
        self._send_message_to_server(url, data)

    def verify_move(self, name, move, amount, chips_left, msg):
        '''This can be overridden to verify if there was a discrepancy between
        what the player sent, and Smithers interpretted it as. This should be 
        sufficient though.'''
        if (name != self.name or
            move != self._last_move[0] or
                (move != "FOLD" and
                 amount != self._last_move[1])):
            print "<bot_framework.py>: Warning - Discrepancy:"
            print "<bot_framework.py>:     Sent %s " % (self._last_move,)
            print "<bot_framework.py>:     Received Name: %s, Move: %s, Amount: %s" % (name, move, amount)
            print "<bot_framework.py>:     Original Received: %s" % msg

    @abc.abstractmethod
    def set_up_competitors(self, competitors):
        """CALLED ONCE: Set up competitors after getting list of players for first tournament"""
        pass

    @abc.abstractmethod
    def receive_tournament_start_message(self, players):
        """What to be done when list of all players is received"""
        return

    @abc.abstractmethod
    def receive_move_message(self, player_name, move, amount, chips_left, is_blind):
        """What to be done when a move is received"""
        return

    @abc.abstractmethod
    def receive_hands_message(self, card1, card2):
        """What to be done when hands are received"""
        return

    @abc.abstractmethod
    def receive_board_message(self, board):
        """What to be done when the board is received"""
        return

    @abc.abstractmethod
    def receive_results_message(self):
        """What to be done when the results are received"""
        return

    @abc.abstractmethod
    def receive_broke_message(self, names):
        """What to be done when a player goes broke"""
        return

    @abc.abstractmethod
    def receive_tournament_winner_message(self, name):
        """What to be done when the tournament winner is announced"""
        return

    @abc.abstractmethod
    def on_move_request(self, min_raise, call, pot, current_bet, chips):
        """What to be done when a move is received"""
        return

    def _is_valid_move_type(self, move):
        return (len(move) == 2
                and move[0] in ["RAISE", "RAISE_TO", "CALL", "FOLD"]
                and move[1] >= 0)

    def register(self):
        data = {"name": self.name}
        # TBD. check for errors
        url = self.server_url + '/register/'
        r = self._send_message_to_server(url, data)
        r_json = r.json()
        self._key = r_json["key"]
        self.name = r_json["name"]
        # TBD. check response. name == name; store chips?
        print r.json()

    def play(self):
        self._connect_to_socket()
        while True:
            if self.is_debug == True:
                raw_input("*--------------------*")
            msg = self._get_message_from_socket()
            m_type = msg.get("type", None)

            if m_type == "TOURNAMENT_START":
                players = self._extract_tournament_start(msg)
                if not self.competitors:
                    self.set_up_competitors(
                        [p for p in players if p["name"] != self.name])
                self.receive_tournament_start_message(players)

            elif m_type == "DEALT_HANDS":
                card_tuple = self._extract_hand(msg)
                if card_tuple is not None:
                    card1, card2 = card_tuple
                    self.receive_hands_message(card1, card2)
                elif not self._is_bust:
                    self._is_bust = True
                    print "<bot_framework.py>: Warning - Gone Bust"

            elif m_type == "DEALT_BOARD":
                board, pot = self._extract_board(msg)
                self.receive_board_message(board, pot)

            elif m_type == "BLIND":
                name, move, bet, chips_left = self._extract_move(msg)
                self.receive_move_message(name, move, bet, chips_left, True)

            elif m_type == "MOVE":
                name, move, bet, chips_left = self._extract_move(msg)
                if name == self.name:  # just sent in move. check it
                    self.verify_move(name, move, bet, chips_left, msg)
                else:
                    self.receive_move_message(
                        name, move, bet, chips_left, False)

            elif m_type == "RESULTS":
                results_list = self._extract_results(msg)
                self.receive_results_message(results_list)

            elif m_type == "BROKE":
                broke = self._extract_broke(msg)
                self.receive_broke_message(broke)

            elif m_type == "WINNER":
                winner = self._extract_winner(msg)
                self.receive_tournament_winner_message(winner)

            elif m_type == "PING":
                self.socket.send("PONG")

            elif m_type == "MOVE_REQUEST":
                if msg.get("name", None) == self.name:
                    if self.is_test:
                        raw_input("move requested for %s" % self.name)
                    min_raise, call, pot, current_bet, chips = self._extract_move_request(
                        msg)
                    move = self.on_move_request(
                        min_raise, call, pot, current_bet, chips)
                    self._send_move_to_server(move)
                    self._last_move = move

            elif m_type == "SHUTDOWN":
                self.socket.close()
                return

            else:
                pass


if __name__ == "__main__":
    name = raw_input('Enter RAW BOTNAME: ')
    pb = PokerBotFramework(name, "http://localhost:6767",
                           "tcp://127.0.0.1:9950")
    pb.register()
    # pb.is_debug = True
    pb.play()
