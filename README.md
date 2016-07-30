***Smithers is a poker server to help all your rich poker bots lose all their money together.***


##Tech:
+ implemented in c++
+ mongrel2 for server
+ zeromq for interprocess communication

##Directory Structure:
* **src** - smithers source code
* **clients** - hosts some python implementations of a bare poker bot skeleton

##Requirements & Set Up:
###Smithers Server (cpp)
```
DEPENDENCY GRAPH:
Smithers
├── JsonCpp (step 2)
├── ZMQCpp  (3)
└── Mongrel2-Cpp (4)
    ├── Json-c  # for now
    └── ZMQCpp 
```
----------
#####1. Download Mongrel2
```
    brew install mongrel2
```

#####2. Download Json.Cpp & amalgamate into a file
```
    git clone https://github.com/open-source-parsers/jsoncpp/ jsoncpp
    cd jsoncpp
    python amalgamate.py   # generates jsoncpp/dist/ directory
    cd ..
```
 
#####3. Download ZeroMQ cpp bindings
```
    git clone https://github.com/zeromq/cppzmq cppzmq
```

#####4. Download Mongrel2 cpp bindings & build library

- *Mongrel2 depend on json-c for now, not jsoncpp - instructions to install on page*
- *Also requires the ZeroMQ header file* 

```
   git clone https://github.com/condnsdmatters/mongrel2-cpp mongrel2-cpp
   cd mongrel2-cpp
   make all  # builds lib2mpp.a
   cd ..
```

#####5. Make build & runtime dependencies available
- Make the following tree
- *You will need: `jsoncpp/dist/json.cpp` & `jsoncpp/dist/json/`*
- *You will need: `cppzmq/zmq.hpp`*
- *You will need: `mongrel2-cpp/lib2mpp.a` and `mongrel2-cpp/lib/m2pp\*.hpp` headers*

```
    ADD TO SMITHERS DIRECTORY
    logs/
    run/
    external/
    ├── include
    │   ├── json
    │   │   ├── json-forwards.h
    │   │   └── json.h
    |   ├── zmq.hpp
    │   ├── m2pp.hpp
    │   └── m2pp_internal.hpp
    └── lib
        └── libm2pp.a

```


#####6. You're now ready to build Smithers. `make all`

#####7. You are now ready to run Smithers.

##Running Smithers
#####1. Generate the sqlite database that mongrel uses as its configuration

```
    m2sh load -config pbb_mongrel.conf -db pbb_mongrel.sqlite   
```

#####2. Run the server  

```
    m2sh start -host localhost
```

#####3. Run Smithers. Usage:

```
    `./smithers.tsk` # [to be filled in]
```

##Setting up & Running a Test Poker Bot
#####1. Install Requirements

```
    pip install pyzmq
    pip install # sort this out
```

#####2. Run some bots

```
    python # [to be filled in] 
```

##Playing Poker
1. What does smithers do?
2. Where do the clients listen?
3. What messages does it send/receive
Requests:



