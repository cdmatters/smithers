#Smithers
***Smithers is a poker server to help all your rich poker bots lose all their money together.***


##Tech:
+ implemented in c++ (for masochistic reasons)
+ mongrel2 for server, mongrel2-cpp bindings used for cpp
+ zeromq for interprocess communication

##Directory Structure:
* **src** - smithers source code
* **clients** - hosts some python implementations of a bare poker bot skeleton

##Requirements & Set Up:
Batteries not included. Follow instructions to get your batteries.
```
DEPENDENCY GRAPH:
Smithers
├── jsoncpp (step 2)
├── cppzmq  (3)
└── mongrel2-cpp (4)
    ├── json-c   # to be moved to jsoncpp
    └── cppzmq 
```
----------
####1. Install Mongrel2
```
    brew install mongrel2
```

####2. Get Json.Cpp & generate compact header + source
```
    git clone https://github.com/open-source-parsers/jsoncpp/ jsoncpp
    cd jsoncpp && python amalgamate.py && cd ..  
```
 
####3. Get ZeroMQ cpp bindings
```
    git clone https://github.com/zeromq/cppzmq cppzmq
```

####4. Get Mongrel2 cpp bindings & build lib2mpp.a library

- *follow build instructions on repo's readme*
- *alse requires the cppzmq header file: zmq.hpp* 

```
   git clone https://github.com/condnsdmatters/mongrel2-cpp mongrel2-cpp
   cd mongrel2-cpp
   make all  # builds lib2mpp.a
   cd ..
```

####5. Make dependencies available to Smithers
- If you dont want to install all the above directories properly, Smithers will build with the following tree available in root.
```
    ADD TO SMITHERS DIRECTORY (with where to find files)
    logs/
    run/
    obj/
    └─── ext/
    external/
    ├── include                
    │   ├── json         - - - - - - - - -> jsoncpp/dist/json/
    │   │   ├── json-forwards.h
    │   │   └── json.h
    |   ├── zmq.hpp      - - - - - - - - -> cppzmq/zmq.hpp
    │   ├── m2pp.hpp     - - - - - - - - -> mongrel2-cpp/lib/m2pp.hpp
    │   └── m2pp_internal.hpp  - - - - - -> mongrel2-cpp/lib/m2pp_internal.hpp
    ├── lib
    │   └── libm2pp.a    - - - - - - - - -> mongrel2-cpp/lib2mpp.a
    └── src
        └── jsoncpp.cpp  - - - - - - - - -> jsoncpp/dist/jsoncpp.cpp
```


####6. Build Smithers!
```
make all
```

##Running Smithers
####1. Generate the sqlite database that mongrel uses as its configuration

```
    m2sh load -config pbb_mongrel.conf -db pbb_mongrel.sqlite   
```

####2. Run the server  

```
    m2sh start -host localhost
```

####3. Run Smithers. Usage:

```
    # usage:  ./smithers.tsk  <p> <l> <t> <c> <m> <r>
    #     where: p - player_bots, l - web socket listeners, t - tournaments
                 c - chips, m - min raise, r - min raise doubles after r hands
    eg: ./smithers.tsk 7 7 10 10000 200 20
```

##Setting up & Running a Test Poker Bot
####1. Install Python Requirements

```
    cat requirements | xargs pip install
```

####2. Run a single test bot
The test bot is the minimal implementation based on the bot framework. If you're wanting to build a bot you're best off forking/cloning the smithers--client repo, or just copying test_bot.py and bot_framework.py and running from this

```
    python test_bot.py <name> # name of the bot
```

####3. Run a whole suite of bots
There are a couple of utility files in the clients repo that can be used to test the output of smithers
- `python trial_game.py <int>` uses multiprocessing to set up `<int>` number of players to run in a tournament
- `python listener.py <MESSAGE_TYPE>` will sign up as a listener and print the raw messages json from Smithers, filtering out messages with type=="MESSAGE_TYPE"  (eg 'MOVE' or 'MOVE_REQUEST')

##Playing Poker
1. What does smithers do?
    - Smithes deals cards out to you, and sends out a move request every time it needs a move from a player. If the player sends back something wrong ('raise' more than chips the bot has) Smithers corrects aiming to keep the game going (ie it will take that as 'all-in').
    - The communication takes place largely over websockets, though historically it was via normal sockets and HTTP post requests.

2. Where do the clients listen?
    - Clients listen by connecting to the websocket address, with extra url '/watch/'. All the required communication (including manual pings & pongs) implemented in the bot + framework

3. What messages does it send/receive
    - Smithers and the bots communicate via json. The move types can be seen being constructed in messages.h/.cpp, or in the play segment of the bot framework. All you need to know is that every message has been faithfully passed on from the framework to the test_bot.py



