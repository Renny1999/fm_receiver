CC = g++
SOAPYFLAGS = -lSoapySDR
FFTWFFLAGS = -lfftw3f
CFLAGS = -DEBUG
DEBUG = -DEBUG

all: main.cpp ./utils/utils.cpp ./threads/*thread.cpp
	clear
<<<<<<< HEAD
	$(CC) -pthread -o ../target/main $? $(SOAPYFLAGS) $(FFTWFFLAGS) $(CFLAGS)
=======
	$(CC) -pthread -o ../target/main $? $(SOAPYFLAGS) $(FFTWFFLAGS)
>>>>>>> 0a5d732e234aeb8651a26b3762144fe895dceeac

debug: main.cpp ./utils/utils.cpp ./threads/*thread.cpp
	clear
	$(CC) -g -pthread -o ../target/debug $? $(SOAPYFLAGS) $(FFTWFFLAGS) $(DEBUG)

test:  test.cpp ./utils/utils.cpp ./threads/*thread.cpp
	$(CC) -pthread -o ../target/test  $? $(SOAPYFLAGS) $(FFTWFFLAGS) $(DEBUG)

run: 
	make reset || true
	make all
	./../target/main

reset:
	rm ./output/exp/*
	rm ./log.txt || true
