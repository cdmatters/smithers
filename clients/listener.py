import requests
import zmq

#from  mongrel2.handler import Connection 
import time
import json
import sys

def listener():
    port = "9950"
    context = zmq.Context()
    socket = context.socket(zmq.SUB)

    socket.connect ("tcp://127.0.0.1:%s" % port)
    socket.setsockopt(zmq.SUBSCRIBE, '')

    total_value = 0
    while True:
        message = socket.recv()
        print message

if __name__ == "__main__":
    listener()