CXXFLAGS = -g -Wall -std=c++11

default: all

all: crc_tx crc_rx crc_vs_parity2d

crc_tx: crc_tx.o crc_encoder.o
	g++ $(CXXFLAGS) -o crc_tx $^

crc_rx: crc_rx.o crc_encoder.o
	g++ $(CXXFLAGS) -o crc_rx $^

crc_vs_parity2d: crc_vs_parity2d.o crc_encoder.o parity2d_encoder.o
	g++ $(CXXFLAGS) -o crc_vs_parity2d $^

crc_tx.o: crc_encoder.h crc_tx.cpp
	g++ $(CXXFLAGS) -c -o crc_tx.o crc_tx.cpp

crc_rx.o: crc_encoder.h crc_rx.cpp
	g++ $(CXXFLAGS) -c -o crc_rx.o crc_rx.cpp

crc_vs_parity2d.o: crc_encoder.h parity2d_encoder.h crc_vs_parity2d.cpp
	g++ $(CXXFLAGS) -c -o crc_vs_parity2d.o crc_vs_parity2d.cpp

crc_encoder.o: crc_encoder.h crc_encoder.cpp
	g++ $(CXXFLAGS) -c -o crc_encoder.o crc_encoder.cpp

parity2d_encoder.o: parity2d_encoder.h parity2d_encoder.cpp
	g++ $(CFLCXXFLAGSAGS) -c -o parity2d_encoder.o parity2d_encoder.cpp

clean:
	rm *.o