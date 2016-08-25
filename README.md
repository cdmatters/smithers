#Smithers
***Smithers is a poker server to help all your rich poker bots lose all their money together.***

#Contents
1. [What?](#what)
2. [Tech](#technology)
3. [Repo Structure](#repo-structure)
4. [Installation](#installation)
    + [Dependencies](#dependencies)
    + [Quick Install](#quick-install)
5. [Playing a Game](#playing-a-game)
    + [Running Smithers](#running-smithers)
    + [Running the Bots](#running-the-bots)
6. [Future Work](#future-work)


##What?
Smithers is a server that allows poker bots to play poker against one another, very quickly.  It is implemented in C++, partly as an exercise, partly as a learning opportunity in learning a low level stack, and partly because it's fast. 

In terms of its structure Smithers is composed of a configuration for a mongrel2 server (which hooks up the http/websockets requests to ZeroMQ endpoints) and an executable which communicates with those endpoints and effectively runs the game.

A game of poker is effectively run by having the bots listening on web sockets (or raw sockets), and being sent (publically) both: 1) other players actions and 2) the servers own request for an action to be taken by that same player. For example, the server will send a move request asking for a move from "BotA", and then publically send that move on to other players (after having processed it), before moving to "BotB". Some basic error checking is done, and on these cases Smithers always resolves errors by trying to keep the overall game going (ie if raise > chips -> all in). 

A test bot (`monty.py`) is also provided to provide a sample skeleton of a working bot, in python. This relies on a class of an inherited BotFramework that handles all the communications plumbing for you (`smithers-framework.py`). The framework requires the implementation of basic callbacks in an inherited class (ie what to do if you receive a notification that another player has raised) before it can be instantiated. This is designed to be as simple as possible, and make the writing of poker bots as elegant as possible: at its very least, all you need to do is implement one function; and at most for a complex bot, you're fully covered with just nine.

Anyone giving this more than a quick lookover will notice that its not very secure. It's not meant to be. Its run amongst friends who are not going to spoof each other, MITM each other or look at each others cards. An encryption mechanism may be on the cards (pardon the pun) in the future, but its not a high priority. If it is for you, see the "Future Work" section and submit a pull request! :)

##Technology
+ implemented in c++ (for masochistic reasons)
+ mongrel2 for server, mongrel2-cpp bindings used for cpp.
+ zeromq for interprocess communication with raw sockets. 
+ otherwise (mainly) use websockets & http post over websockets
+ messages are sent in json

##Repo Structure
* **src** - smithers executable source code
* **clients** - python implementations of basic bot clients and python utils
* **.** - configs and makefiles

##Installation
This is a .cpp project so, natch, there aren't a lot of batteries included in the project and you'll have to install a few packages or libraries. The easiest way s to follow these instructions and build a working directory displayed at the bottom. This should allow the Makefile to find anything you need without installing anything permanent on your system.
####Dependencies
Smithers depends on the following packages
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
####Quick Install
1. Install Mongrel2 & Clone Dependencies

    ``` 
    # 1. Install mongrel2
    brew install mongrel2

    # 2. Clone necessary repos
    git clone https://github.com/open-source-parsers/jsoncpp/ jsoncpp

    git clone https://github.com/zeromq/cppzmq cppzmq

    git clone https://github.com/condnsdmatters/mongrel2-cpp mongrel2-cpp
    ```

2. Generate jsoncpp amalgamated header + source
    ```
    cd jsoncpp && python amalgamate.py && cd ..  
    ```

3. Build mongrel2cpp's  lib2mpp.a library
    - *follow build instructions on repo's readme*
    - *requires the header file: zmq.hpp from cppzmq above* 

    ```
    cd mongrel2-cpp && make all && cd ..
    ```

4. Make dependencies available to Smithers
    - If you dont want to install all the above directories properly, Smithers will build with the following tree available in root (dont forget to make the empty directories!)

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

5. Build Smithers!
    ```
    make all
    ```

##Playing a Game
#### Running Smithers
1. Generate the sqlite database that mongrel uses as its configuration.

    ```
    m2sh load -config pbb_mongrel.conf -db pbb_mongrel.sqlite   
    ```

2. Run the server

    ```
    m2sh start -host localhost -db pbb_mongrel.sqlite 
    # m2sh stop -host localhost
    ```
    
3. Run Smithers. 

    ```
    # usage:  ./smithers.tsk  <p> <l> <t> <c> <m> <r>
    #     where: p - player_bots, l - web socket listeners, t - tournaments
                 c - chips, m - min raise, r - min raise doubles after r hands
    eg: ./smithers.tsk 7 7 10 10000 200 20
    ```

#### Running the Bots
1. Install Python Requirements (once only)

    ```
    cat requirements | xargs pip install
    ```

2. Run a single test bot
    - monty.py runs a bot that randomly "min raises", "calls" or "folds"

    ```
    python monty.py <name> # name of the bot
    ```

3. Run a whole suite of bots, using the files in clients/utils from client directory.
    - `python -m utils.trial_game <int>` 
        - uses multiprocessing to set up `<int>` number of players to run in a tournament in parallel
    - `python -m utils.listener <MESSAGE_TYPE>` 
        - will sign up as a listener and print the raw messages json from Smithers, filtering out messages by message type

##Future Work
- "OMG but the communication is so unsafe??!"
- "Lol why would you use Mongrel2, X is so much better??!"
- "EWww there are no timeouts - why arent the listeners running in a separate thread??!!"
- "NOOB Wheres the error correction if the client disconnects??"
- "Why havent you done... "..etc 

If this sounds like you I'm thankful you've taken an interest in ameliorating my project, and I'm more than happy to look at pull requests for improvements... Thanks in advance!

If this doesnt sound like you then thanks also for reading this far, and hopefully you'll enjoy using this software, and building your own poker bots (which after all, is where all the fun's at). Have a look at the [smithers--monty](https://github.com/condnsdmatters/smithers--monty) repo over the coming weeks to see the bots I come up with! Thanks! X



