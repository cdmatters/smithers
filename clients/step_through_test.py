import requests
import zmq

#from  mongrel2.handler import Connection 
import time
import json
import sys

def get_players_list():
    return [
        { 
            "name": "JUPITER",
            "move": "RAISE_TO",
            "amount": 1000
        },{
            "name": "VENUS",
            "move": "CALL",
            "amount": 1000
        },{
            "name": "MARS",
            "move": "FOLD",
            "amount": 1000
        }
    ]


def register_me(name):
    data = {"name":name}
    r = requests.post( 'http://localhost:6767/register/', json=data )
    print r.json()

def subscribe_me(player_list):
    names = {p["name"]:p for p in player_list}

    port = "9950"
    context = zmq.Context()
    socket = context.socket(zmq.SUB)

    socket.connect ("tcp://127.0.0.1:%s" % port)
    socket.setsockopt(zmq.SUBSCRIBE, '')

    total_value = 0
    while True:
        message = socket.recv()
        print message
        json_message = json.loads(message);
        raw_input(" message loaded ")
        if json_message["type"]=='MOVE_REQUEST' and json_message["name"] in names:
            player_name = json_message["name"]
            raw_input("move request: %s" % player_name) 
            print "posting message"
            next_move = {
                'name':player_name,
                'move':names[player_name]["move"],
                'chips':names[player_name]["amount"]
            }
            r = requests.post( 'http://localhost:6767/move/', json=next_move )
       


def main():
    players = get_players_list()
    for p in players:
        register_me(p["name"])
    subscribe_me(players)

if __name__ == "__main__":
    main()