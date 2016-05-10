import requests
import zmq

#from  mongrel2.handler import Connection 
import time
import json
import sys


def register_me(name):
    r = requests.post( 'http://localhost:6767/register/', data={'name':name} )
    print r.json()

def subscribe_me(name):

    port = "9950"
    context = zmq.Context()
    socket = context.socket(zmq.SUB)

    print "Collecting updates from weather server..."
    socket.connect ("tcp://127.0.0.1:%s" % port)
    socket.setsockopt(zmq.SUBSCRIBE, '')

    total_value = 0
    while True:
        message = socket.recv()
        print message
        json_message = json.loads(message);
        print json_message

        if json_message["type"]=='MOVE_REQUEST' and json_message["name"]==name:
            print "posting message"
            r = requests.post( 'http://localhost:6767/move/', data={'move':'raise', 'name':name} )



def main():
    name = sys.argv[1]
    register_me(name)
    subscribe_me(name)

    # sender_id = "82209006-86FF-4982-B5EA-D1E29E55D481"
    # conn = Connection(sender_id,'tcp://127.0.0.1:9999','tcp://127.0.0.1:9998')

    # print '...connected'
    # while True:
    #     req = conn.recv()
       
    #     if req.is_disconnect:
    #         print '== disconnect =='
        
    #     if req.body != '{"type":"disconnect"}':
    #         print dir(req.headers), req.headers, req.conn_id
    #         resp = []
    #         resp.append("<pre> %s")
    #         resp.append("SENDER: %s " % req.sender )
    #         resp.append("IDENT: %s " % req.conn_id )
    #         resp.append("PATH: %s " % req.path )
    #         resp.append("BODY: %s " % req.body )
    #         resp.append("/<pre> %s\n")

    #         response = ''.join(resp)
    #         for i in xrange(10):
    #             str(i)

    #             conn.reply_websocket(req, response)
    #             conn.reply_websocket(req, '<pre>%s</pre>\n'%i)
    #             time.sleep(1)
            
    #         conn.close(req)
    #         print response



if __name__ =='__main__':
    main()



