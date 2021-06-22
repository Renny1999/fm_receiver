#ifndef PILOT_EXTRACTION_THREAD_H
#define PILOT_EXTRACTION_THREAD_H

#include <complex>
#include <string>

#include "../utils/BlockingQueue.h"

struct pilot_extract_args_1{
	BlockingQueue<double>* in;
	BlockingQueue<std::complex<double>>* out;
	std::string filter_path_diffeq_a;
	std::string filter_path_diffeq_b;
	std::string filter_path_h;
	double sample_rate;
	int chunk_size;
	int dec_rate;
	int taps;
};
struct pilot_extract_args_2{
	BlockingQueue<std::complex<double>>* in;
	BlockingQueue<std::complex<double>>* out;
	std::string filter_path_diffeq_a;
	std::string filter_path_diffeq_b;
	std::string filter_path_h;
	double sample_rate;
	int chunk_size;
	int dec_rate;
	int taps;
};

void* pilot_extraction_thread_stage_1_diffeq(void* args);
void* pilot_extraction_thread_stage_2_diffeq(void* args);

#endif