#ifndef LR_DIFF_RECOVERY_THREAD_H
#define LR_DIFF_RECOVERY_THREAD_H

#include <string>
#include <complex>
#include "../utils/BlockingQueue.h"


struct LR_diff_recovery_args{
	BlockingQueue<double>* in;
	BlockingQueue<std::complex<double>>* out;	

	std::string filter_path_diffeq_a;
	std::string filter_path_diffeq_b;

	int sample_rate;
	int chunk_size;

	int dec_rate;
};

void* LR_diff_recovery_thread(void* args);

#endif