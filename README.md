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
    # sample ports. this needs to be cleared up. this works for the python test
    ./smithers.tsk 'tcp://127.0.0.1:9999' 'tcp://127.0.0.1:9998'
```

##Setting up & Running a Test Poker Bot
####1. Install Requirements

```
    pip install pyzmq
    pip install # sort this out
```

####2. Run some bots (for now)

```
    # this sets up a game of 3 bots, one always RAISES, one CALLS, one FOLDS,
    python step_through_test.py 
```

##Playing Poker
1. What does smithers do?
2. Where do the clients listen?
3. What messages does it send/receive
Requests:



