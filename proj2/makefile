CXXFLAGS = -g -Wall -std=c++11
CXX = g++

default: all

all: servermain client

servermain: servermain.o serverprogm.o
	$(CXX) $(CXXFLAGS) -o servermain $^

servermain.o: servermain.cpp serverprogm.h
	$(CXX) $(CXXFLAGS) -c -o servermain.o servermain.cpp

client: client.o clientprogm.o
	$(CXX) $(CXXFLAGS) -o client $^

client.o: client.cpp clientprogm.h
	$(CXX) $(CXXFLAGS) -c -o client.o client.cpp

serverprogm.o: serverprogm.h serverprogm.cpp
	$(CXX) $(CXXFLAGS) -c -o serverprogm.o serverprogm.cpp

clientprogm.o: clientprogm.h clientprogm.cpp
	$(CXX) $(CXXFLAGS) -c -o clientprogm.o clientprogm.cpp

clean:
	rm *.o
