#include <cstdio>
#include <pthread.h>
#include <iostream>

#include "utils.h"

using  namespace std;

complex<float>* convolution(complex<float>* sig1, int sig1_size, complex<float>* kernel, int kernel_size){

	// int num_threads = sig1_size/kernel_size;
	// if (sig1_size%kernel_size > 0){
	// 	num_threads++;
	// }	

	int num_threads = sig1_size;
	// cout<<"need "<<num_threads<<" threads"<<endl;

	// spawn threads to compute sums faster	
	elementwise_multi_sum_args* args = new elementwise_multi_sum_args[num_threads];
	pthread_t* thread_ids = new pthread_t[num_threads];

	for(int i = 0; i < num_threads; i++){
		// int start_index = i*kernel_size;
		// int end_index = start_index+kernel_size-1;
		int start_index = i;
		int end_index = start_index+kernel_size-1;

		args[i].arr1 = sig1+start_index;
		args[i].arr2 = kernel;
		args[i].startIndex = start_index;
		args[i].endIndex = end_index;
		args[i].maxIndex = sig1_size-1;
		args[i].N = kernel_size;

		pthread_create(&thread_ids[i], NULL, &elementwise_multi_sum, &args[i]);
	}

	// wait for finish
	for(int i = 0; i < num_threads; i++){
		pthread_join(thread_ids[i], NULL);
	}

	// sum up all results
	// complex<float> output(0,0);
	complex<float>* output = new complex<float>[num_threads];
	for(int i = 0; i < num_threads; i++){
		output[i] = *(args[i].res);
	}

	return output;
}

void* elementwise_multi_sum(void* args){
	elementwise_multi_sum_args* params = (elementwise_multi_sum_args*) args;

	complex<float>* sig = params->arr1;
	complex<float>* kernel = params->arr2;

	int N = params->N;

	int start_i = params->startIndex;
	int end_i = params->endIndex;
	int max_i = params->maxIndex;
	// printf("thread for %d ~ %d, max=%d\n", start_i, end_i);

	complex<float>* output = new complex<float>(0,0);

	for(int i = 0; i <= end_i; i++){
		int currentIndex = start_i + i;
		if(currentIndex > max_i){
			continue;
		}else{
			// the filter is reversed
			*output += sig[i]*kernel[N-1-i];
			// cout<<sig[i] << " * "<< kernel[N-1-i]<<endl;
		}
	}

	params->res = output;
}