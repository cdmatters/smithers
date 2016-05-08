CXX=g++
CXXFLAGS=-Wall -Wextra -Ilib
LIBS=-lm2pp -lzmq -ljson 

prefix=/usr/local
INCDIRS= -I$(prefix)/include -I./external/include
LIBDIRS= -L$(prefix)/lib -L./external/lib

BOTBATTLE=pbbattle

all:
	$(CXX) -o $(BOTBATTLE).tsk $(CXXFLAGS) $(LIBDIRS) $(INCDIRS) main.m.cpp $(LIBS)


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) $(BOTBATTLE).tsk $(BOTBATTLE).o 