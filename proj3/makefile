CXXFLAGS = -g -Wall -std=c++11
CXX = g++

default: all

all: serverA serverB servermain clean

serverA: serverA.o backendprog.o
	$(CXX) $(CXXFLAGS) -o serverA $^

serverA.o: serverA.cpp backendprog.h
	$(CXX) $(CXXFLAGS) -c -o serverA.o serverA.cpp

serverB: serverB.o backendprog.o
	$(CXX) $(CXXFLAGS) -o serverB $^

serverB.o: serverB.cpp backendprog.h
	$(CXX) $(CXXFLAGS) -c -o serverB.o serverB.cpp

servermain: servermain.o servermainprog.o
	$(CXX) $(CXXFLAGS) -o servermain $^

servermain.o: servermain.cpp servermainprog.h
	$(CXX) $(CXXFLAGS) -c -o servermain.o servermain.cpp

backendprog.o: backendprog.h backendprog.cpp
	$(CXX) $(CXXFLAGS) -c -o backendprog.o backendprog.cpp

servermainprog.o: servermainprog.h servermainprog.cpp
	$(CXX) $(CXXFLAGS) -c -o servermainprog.o servermainprog.cpp

clean:
	rm *.o
