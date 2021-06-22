#ifndef LR_DIFF_EXTRACTION_THREAD_H
#define LR_DIFF_EXTRACTION_THREAD_H

#include <string>
#include <complex>
#include "../utils/BlockingQueue.h"

struct LR_extract_args{
	BlockingQueue<std::complex<double>>* LR_diff;
	BlockingQueue<std::complex<double>>* pilot;
	BlockingQueue<std::complex<double>>* out;	

	std::string filter_path_h;
	std::string filter_path_diffeq_a;
	std::string filter_path_diffeq_b;

	int sample_rate;
	int chunk_size;
	int dec_rate;
};


/**
 * moves the LR_diff spectrum to the baseband, 
 * applies a 15kHz lowpass filter then decimates to 48kHz
*/
void* LR_extract_thread(void* args);

#endif