CXX=g++
CXXFLAGS=-Wall -Wextra -Ilib -MMD
LIBS=-lm2pp -lzmq -ljson 

CPP_FILES:= $(wildcard src/*.cpp)
OBJ_FILES:= $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

prefix=/usr/local
INCDIRS= -I$(prefix)/include -I./external/include -I./src -I.
LIBDIRS= -L$(prefix)/lib -L./external/lib

BOTBATTLE=pbbattle

all: main.m.cpp  $(OBJ_FILES)
	$(CXX) -o $(BOTBATTLE).tsk $(CXXFLAGS) $(LIBDIRS) $(INCDIRS)  $(LIBS) $^

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCDIRS) -o $@ -c $<

clean:
	$(RM) $(BOTBATTLE).tsk $(BOTBATTLE).o 

-include $(OBJFILES:.o=.d)