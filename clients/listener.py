import requests
import zmq

#from  mongrel2.handler import Connection 
import time
import json
import sys

def listener(filtered):
    port = "9950"
    context = zmq.Context()
    socket = context.socket(zmq.SUB)

    socket.connect ("tcp://127.0.0.1:%s" % port)
    socket.setsockopt(zmq.SUBSCRIBE, '')

    total_value = 0
    while True:
        message = socket.recv()
        json_message = json.loads(message);
        if json_message["type"] not in filtered:
            print message
            raw_input()

if __name__ == "__main__":
    filtered = sys.argv[1:]
    listener(filtered)