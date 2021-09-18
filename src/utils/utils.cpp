#include <cstdio>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

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
	return nullptr;
}

vector<complex<float>>* read_complex_float_coeffs(string filepath){
	ifstream file;
	file.open(filepath);

	if(!file.is_open()){
		printf("Failed to open %s\n", filepath.c_str());
		file.close();
		exit(-1);
	}
	
	vector<complex<float>>* vec = new vector<complex<float>>();
	string temp;
	while(!file.eof()){
		getline(file,temp);
		float real_part = stof(temp);
		getline(file,temp);
		float imag_part = stof(temp);
		vec->push_back(complex<float>(real_part, imag_part));
	}

	return vec;
}

vector<complex<double>>* read_complex_double_coeffs(string filepath){
	ifstream file;
	file.open(filepath);

	if(!file.is_open()){
		printf("Failed to open %s\n", filepath.c_str());
		file.close();
		exit(-1);
	}
	
	vector<complex<double>>* vec = new vector<complex<double>>();
	string temp;
	while(!file.eof()){
		getline(file,temp);
		double real_part = stof(temp);
		getline(file,temp);
		double imag_part = stof(temp);
		vec->push_back(complex<double>(real_part, imag_part));
	}

	return vec;
}

vector<float>* read_float_coeffs(string filepath){
	ifstream file;
	file.open(filepath);

	if(!file.is_open()){
		printf("Failed to open %s\n", filepath.c_str());
		file.close();
		exit(-1);
	}
	
	vector<float>* vec = new vector<float>();
	string temp;
	while(!file.eof()){
		getline(file,temp);
		float real_part = stof(temp);
		getline(file,temp);
		// float imag_part = stof(temp);
		vec->push_back(real_part);
	}

	return vec;
}

vector<double>* read_double_coeffs(string filepath){
	ifstream file;
	file.open(filepath);

	if(!file.is_open()){
		printf("Failed to open %s\n", filepath.c_str());
		file.close();
		exit(-1);
	}
	
	vector<double>* vec = new vector<double>();
	string temp;
	while(!file.eof()){
		getline(file,temp);
		double real_part = stof(temp);
		getline(file,temp);
		// float imag_part = stof(temp);
		vec->push_back(real_part);
	}

	return vec;
}

int16_t float2int16(float f){
	float sample = f*32768*0.8;
	if(sample > 32767) sample = 32767;
	if(sample < -32768) sample = -32768;
	int16_t integer = (int16_t) sample;

	return integer;
}

int setup_socket(int port){
    int server_fd, new_socket, valread;
    int PORT = 4500;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0){
        cerr<<"failed to open socket"<<endl;
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, 
            SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cerr<<"error setsockopt"<<endl;
        exit(-1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr *) &address, sizeof(address))<0){
        cerr<<"port binding failed"<<endl;
        exit(-1);
    }

    if (listen(server_fd, 3) < 0){
        cerr<<"error listening"<<endl;
        exit(-1);
    }

    cout<<"listening"<<endl;
    if ((new_socket = accept(
                        server_fd,
                        (struct sockaddr*) &address,
                        (socklen_t*) &addrlen))<0)
    {
        cerr<<"error accept"<<endl;
        exit(-1);
    }

    cout<<"client connected"<<endl;
    return new_socket;
}

std::vector<std::string> split(std::string str, char delim){
	vector<std::string> output;
	int lastDelimIndex = -1;
	for(int i = 0; i < str.length(); i++){
		if(str[i] == delim){
			output.push_back(str.substr(lastDelimIndex+1, i-lastDelimIndex-1));
			lastDelimIndex = i;
		}
	}
	output.push_back(str.substr(lastDelimIndex+1, str.length()-lastDelimIndex-1));
	return output;
}
