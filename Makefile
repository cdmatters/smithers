CXX=g++
CXXFLAGS=-Wall -Wextra -Ilib -MMD -std=c++11
LIBS=-lm2pp -lzmq -ljson 

CPP_FILES:= $(wildcard src/*.cpp)
OBJ_FILES:= $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

EXTERNAL_CPP_FILES:= $(wildcard external/src/*.cpp)
EXTERNAL_OBJ_FILES:= $(addprefix obj/ext/,$(notdir $(EXTERNAL_CPP_FILES:.cpp=.o)))

prefix=/usr/local
INCDIRS= -I$(prefix)/include -I./external/include -I./src -I.
LIBDIRS= -L$(prefix)/lib -L./external/lib

BOTBATTLE=pbbattle

all: main.m.cpp $(EXTERNAL_OBJ_FILES) $(OBJ_FILES) 
	$(CXX) -o $(BOTBATTLE).tsk $(CXXFLAGS) $(LIBDIRS) $(INCDIRS)  $(LIBS) $^

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCDIRS)  -o $@ -c $<

obj/ext/%.o: external/src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCDIRS)  -o $@ -c $<

clean:
	$(RM) $(BOTBATTLE).tsk $(BOTBATTLE).o  obj/*.o obj/*.d obj/ext/*.o obj/ext/*.d

-include $(OBJFILES:.o=.d)