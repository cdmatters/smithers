from test_bot import PokerBot
import sys
from multiprocessing import Process, Pool

players = [
    "MARS",
    "VENUS",
    "JUPITER",
    "SATURN",
    "JUNO",
    "MERCURY",
    "HADES",
    "NEPTUNE",
    "APOLLO",
    "MINERVA",
    "POSEIDON"
]

def make_bot_play(name):
    try:
        pb = PokerBot(name, "http://localhost:6767") #  , "tcp://127.0.0.1:9950")
        pb.register()
        pb.play()
        return "WIN"
    except Exception as e:
        print e
        print "DONE"
        return "NOPE"
        sys.exit(1)

if __name__=="__main__":
    player_no = int(sys.argv[1])
    p = Pool(player_no)
    try:
        print(p.map_async(make_bot_play, players[:player_no]).get(9999999))
    except Exception as e:
        print "INTERRUPT ", e
    # processes = []
    # for i in xrange(player_no):
    #     processes.push()

