CC = g++
SOAPYFLAGS = -lSoapySDR
FFTWFFLAGS = -lfftw3f
MACROS = RAWCAPTURE

all: main.cpp ./utils/utils.cpp ./threads/*thread.cpp
	clear
	$(CC) -pthread -O2 -o ../target/main $? $(SOAPYFLAGS) $(FFTWFFLAGS)

debug: main.cpp ./utils/utils.cpp ./threads/*thread.cpp
	clear
	$(CC) -g -pthread -o ../target/debug $? $(SOAPYFLAGS) $(FFTWFFLAGS) -D $(MACROS)

test:  test.cpp ./utils/utils.cpp ./threads/*thread.cpp
	$(CC) -pthread -o ../target/test  $? $(SOAPYFLAGS) $(FFTWFFLAGS) -D $(MACROS)

capture: main.cpp ./utils/utils.cpp ./threads/*thread.cpp
	clear
	$(CC) -D $(MACROS) -pthread -O2 -o ../target/capture $? $(SOAPYFLAGS) $(FFTWFFLAGS) 

run: 
	make reset || true
	make all
	./../target/main

reset:
	rm ./output/exp/*
	rm ./log.txt || true
