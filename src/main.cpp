#include <cstdio>
#include <cstdlib>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include <fstream>

#include "./utils/BlockingQueue.h"
#include "./utils/utils.h"
#include "./threads/capture_thread.h"
#include "./threads/stage_1_filtering_thread.h"
#include <vector>
#include <map>

#include <iostream>

using namespace std;

int CHUNK_SIZE = 512;

void filter_and_save(string output_filename);

int main(){
    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;
    BlockingQueue capture_out;  // same as filter_in
    BlockingQueue stage1_out;


    capture_args capture_config = capture_args();
        capture_config.sample_rate = 1e6;
        capture_config.center_freq = 91.9e6;
        capture_config.out = &capture_out;
        capture_config.chunk_size = CHUNK_SIZE;

    stage_1_filter_args stage_1_config = stage_1_filter_args();
        stage_1_config.in = &capture_out;
        stage_1_config.out = &stage1_out;
        stage_1_config.ntaps = 64;
        stage_1_config.filter_path = "./filters/stage_1_filter.txt";
        stage_1_config.chunk_size = CHUNK_SIZE;
      
    pthread_t capture_id;
    pthread_create(&capture_id, NULL, &capture_thread, &capture_config);

    pthread_t stage_1_id;
    pthread_create(&stage_1_id, NULL, &stage_1_filtering_thread, &stage_1_config);

    pthread_join(capture_id, NULL);
    pthread_join(stage_1_id, NULL);

    FILE *fp;
    // string filename = "./output/filtered_result.txt";
    // string filename = "./output/exp/iFFT_1M.txt";
    // fp = fopen(filename.c_str(),"w");
    // if(fp == nullptr){
    //     perror("Error");
    // }

    // for(int j = 0; j < 5000; j++){
    //     cout<<j<<endl;
    //     complex<float>* buffer = stage1_out.pop()->data;
    //     for(int i = 0; i < CHUNK_SIZE; i++){
    //         complex<float> num = buffer[i];
    //         fprintf(fp, "%f,%f\n", num.real(), num.imag());
    //     }
    // }
    // fclose(fp);

    return 0;
}

void filter_and_save(string output_filename){

}

