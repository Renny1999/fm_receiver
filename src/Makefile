CC = g++
SOAPYFLAGS = -lSoapySDR
FFTWFFLAGS = -lfftw3f

all: main.cpp ./utils/BlockingQueue.cpp ./utils/utils.cpp ./threads/capture_thread.cpp ./threads/stage_1_filtering_thread.cpp
	clear
	$(CC)  -pthread -o main $? $(SOAPYFLAGS) $(FFTWFFLAGS)

test: 
	$(CC) -pthread -o test test.cpp ./utils/BlockingQueue.cpp ./threads/threads.cpp
