CC = g++
SOAPYFLAGS = -lSoapySDR
FFTWFFLAGS = -lfftw3f

all: main.cpp ./utils/utils.cpp ./threads/*thread.cpp
	clear
	$(CC) -g -pthread -o ../target/main $? $(SOAPYFLAGS) $(FFTWFFLAGS)

test: 
	$(CC) -pthread -o ../target/test test.cpp ./utils/BlockingQueue.cpp ./utils/*.cpp
	../target/test

run: 
	make reset || true
	make all
	./../target/main

reset:
	rm ./output/exp/*
	rm ./log.txt || true
