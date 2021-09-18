#ifndef UTILS_H
#define UTILS_H

#include <complex>
#include <vector>
std::complex<float>* convolution(std::complex<float>* sig1, int sig1_size, std::complex<float>* sig2, int sig2_size);

struct elementwise_multi_sum_args{
	std::complex<float>* arr1;
	std::complex<float>* arr2;
	int startIndex;
	int endIndex;
	int maxIndex;
	int N;
	std::complex<float>* res;
};
void* elementwise_multi_sum(void* args);

std::vector<std::complex<float>>* read_complex_float_coeffs(std::string filename);
std::vector<std::complex<double>>* read_complex_double_coeffs(std::string filename);
std::vector<float>* read_float_coeffs(std::string filename);
std::vector<double>* read_double_coeffs(std::string filename);

int setup_socket(int port);

int16_t float2int16(float f);

std::vector<std::string> split(std::string str, char delim);

#endif