CC = g++
SOAPYFLAGS = -lSoapySDR
FFTWFFLAGS = -lfftw3f

all: main.cpp ./utils/utils.cpp ./threads/capture_thread.cpp  ./threads/stage_1_filtering_thread.cpp ./threads/FM_demod_thread.cpp ./threads/mono_audio_extraction_thread.cpp
	clear
	$(CC)  -pthread -o ../target/main $? $(SOAPYFLAGS) $(FFTWFFLAGS)

test: 
	$(CC) -pthread -o ../target/test test.cpp ./utils/BlockingQueue.cpp ./utils/*.cpp
	../target/test

run: 
	make reset || true
	make all
	./../target/main

reset:
	rm ./output/exp/*