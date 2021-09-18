#include <iostream>
#include <complex>
#include "./utils/FixedSizedDeque.h"
#include "./utils/Deque.h"
#include "./utils/BlockingQueue.h"
#include "./utils/utils.h"

#include <fstream>

#define complex_f_p complex<float>*

using namespace std;

int CHUNKSIZE = 512;

int main(){
	ifstream in;
	in.open("./output/exp/unfiltered_1M.txt");

	if(!in){
		cout<<"FAILED TO OPEN FILE"<<endl;
		exit(0);
	}

	BlockingQueue<complex<float>> data;

	string line = "";
	int count = 0;
	complex_f_p buffer = new complex<float>[CHUNKSIZE];

	while(getline(in,line)){
		vector<string> v = split(line, ',');
		complex<float> num(stof(v[0]), stof(v[1]));
		buffer[count] = num;
		count++;
		if(count == CHUNKSIZE){
			data.push(buffer);
			buffer = new complex<float>[CHUNKSIZE];
			count = 0;
		}
	}
	// we have read in the data
	// now perform stage 1 filtering

	auto stage_1_filter = *read_complex_float_coeffs("./filters/stage_1_filter_h_100kHz.txt");

	for(complex<float> num : stage_1_filter){
		cout<<num<<endl;
	}

	return 0;
}