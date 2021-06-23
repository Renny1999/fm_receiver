#ifndef LR_DIFF_EXTRACTION_THREAD_H
#define LR_DIFF_EXTRACTION_THREAD_H

#include <string>
#include <complex>
#include "../utils/BlockingQueue.h"

struct LR_diff_extract_args{
	BlockingQueue<std::complex<double>>* LR_diff;
	BlockingQueue<std::complex<double>>* pilot;
	BlockingQueue<double>* out;	

	std::string filter_path_h;
	std::string filter_path_diffeq_a;
	std::string filter_path_diffeq_b;

	int taps;
	int sample_rate;
	int chunk_size;
	int dec_rate;
};


/**
 * moves the LR_diff spectrum to the baseband, 
 * applies a 15kHz lowpass filter then decimates to 48kHz
*/
void* LR_diff_extraction_thread(void* args);

#endif