#ifndef DECIMATION_THREAD_H
#define DECIMATION_THREAD_H

#include "./utils/BlockingQueue.h"

struct decimation_args{
	BlockingQueue<complex<float>>* in;
	BlockingQueue<complex<float>>* out;
	int dec_rate;
	int chunk_size;
};

void* decimation_thread(void* args);

#endif