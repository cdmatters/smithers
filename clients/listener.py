import requests
import zmq

#from  mongrel2.handler import Connection 
import time
import json
import sys

from websocket import create_connection

def listener(filtered):
    # port = "9950"
    # context = zmq.Context()
    # socket = context.socket(zmq.SUB)

    # socket.connect ("tcp://127.0.0.1:%s" % port)
    # socket.setsockopt(zmq.SUBSCRIBE, '')

    socket = create_connection("ws://localhost:6767/watch/")

    total_value = 0
    while True:
        message = socket.recv()
        print message
        json_message = json.loads(message);
        if json_message.get("type", None) not in filtered:
            print message
            raw_input()

if __name__ == "__main__":
    filtered = sys.argv[1:]
    listener(filtered)