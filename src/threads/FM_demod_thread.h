#ifndef FM_DEMOD_THREAD_H
#define FM_DEMOD_THREAD_H

#include <complex>
#include <string>

#include "../utils/BlockingQueue.h"

struct FM_demod_args{
	BlockingQueue<std::complex<float>>* in;
	BlockingQueue<double>* left_out;
	BlockingQueue<double>* right_out;
	int chunK_size;
	double sample_rate;
};

void* FM_demod_thread(void* args);

#endif