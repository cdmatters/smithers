***Smithers is a poker server to help all your rich poker bots lose all their money together.***


##Tech:
+ implemented in c++
+ mongrel2 for server
+ zeromq for interprocess communication

##Directory Structure:
* **src** - smithers source code
* **clients** - hosts some python implementations of a bare poker bot skeleton

##Requirements & Set Up:
####Smithers (cpp)

1. Download Mongrel2
   ```
    brew install mongrel2
   ```

2. Download ZeroMQ cpp bindings
    ```
    git clone https://github.com/zeromq/cppzmq cppzmq
    ```

3. Download Mongrel2 cpp bindings & build library
   ```
   git clone https://github.com/condnsdmatters/mongrel2-cpp mongrel2-cpp
   cd mongrel2-cpp
   make all
   cd ..
   ```

4. Download Json.Cpp & amalgamate into a file
    ```
    git clone https://github.com/open-source-parsers/jsoncpp/ jsoncpp
    cd jsoncpp
    python amalgamate.py
    cd ..
    ```
    
    *(in particular you want to use "Amalgamated Source method they describe")*
5. Make some new directories for both mongrel and to keep the build clean.

    - `mkdir run logs` - for mongrel2
    - `mkdir obj external external/include external/lib` - for smithers build
    - `cp -r ../jsoncpp/dist/json external/include/json` - copy the json headers
    - `cp ../mongrel2-cpp/libm2pp.a external/lib/lib2mpp.a` - copy the mongrel binding library
    - `cp ../mongrel2-cpp/m2pp*.hpp external/include` - copy the mongrel binding headers

6. Place your external dependencies in the external directory
    ```
      $ tree external/
    external/
    ├── include
    │   ├── json
    │   │   ├── json-forwards.h
    │   │   └── json.h
    │   ├── m2pp.hpp
    │   └── m2pp_internal.hpp
    └── lib
        └── libm2pp.a

    ```

7. Build. 

7. You are now ready to run Smithers.

####Clients (py)

1. Install Zeromq

    ```
    pip install pyzmq
    ```

2. Websockets
3. You are now ready to run a trial pokerbot

##Running Smithers
1. Generate the sqlite database that mongrel uses as its configuration

    ```
    m2sh load -config pbb_mongrel.conf -db pbb_mongrel.sqlite   
    ```

2. Run the server  

    ```
    m2sh start -host localhost
    ```

3. Run Smithers. Usage:

    ```
    `./smithers.tsk`
    ```

##Running a Poker Bot
1. Run some test bots. Usage:

    ```
    python #[to be filled in] 
    ```

##Playing Poker
1. What does smithers do?
2. Where do the clients listen?
3. What messages does it send/receive
Requests:



