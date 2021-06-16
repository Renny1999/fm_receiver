#include <cstdio>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include "./utils/BlockingQueue.h"
#include "./utils/utils.h"
#include "./threads/capture_thread.h"
#include "./threads/stage_1_filtering_thread.h"
#include "./threads/FM_demod_thread.h"


using namespace std;

int CHUNK_SIZE = 512;
int Fs = 1e6;

int main(){
    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;
    BlockingQueue<complex<float>> capture_out;  // same as filter_in
    BlockingQueue<complex<float>> stage1_out;   // same as fm_demod_in
    BlockingQueue<double> fm_demod_left_out; 
    BlockingQueue<double> fm_demod_right_out; 


    capture_args capture_config;
        capture_config.sample_rate = Fs;
        capture_config.center_freq = 91.9e6;
        capture_config.out = &capture_out;
        capture_config.chunk_size = CHUNK_SIZE;

    stage_1_filter_args stage_1_config;
        stage_1_config.in = &capture_out;
        stage_1_config.out = &stage1_out;
        stage_1_config.ntaps = 64;
        stage_1_config.filter_path = "./filters/stage_1_filter.txt";
        stage_1_config.sample_rate = Fs;
        stage_1_config.chunk_size = CHUNK_SIZE;

    FM_demod_args fm_demod_config;
        fm_demod_config.in = &stage1_out;
        fm_demod_config.left_out = &fm_demod_left_out;
        fm_demod_config.right_out = &fm_demod_right_out;
        fm_demod_config.chunK_size = CHUNK_SIZE;
        fm_demod_config.sample_rate = 200000;   // calculated as Fs/dec_rate, it is typically 200kHz which is 
                                                // 2 times the bandwidth of a FM radio channel
      
    pthread_t capture_id;
    pthread_create(&capture_id, NULL, &capture_thread, &capture_config);

    pthread_t stage_1_id;
    pthread_create(&stage_1_id, NULL, &stage_1_filtering_thread, &stage_1_config);

    pthread_t fm_demod_id;
    pthread_create(&fm_demod_id, NULL, &FM_demod_thread, &fm_demod_config);

    pthread_join(capture_id, NULL);
    pthread_join(stage_1_id, NULL);
    pthread_join(fm_demod_id, NULL);

    FILE *fp;
    // string filename = "./output/filtered_result.txt";
    string filename = "./output/exp/fm_demodulated.txt";
    fp = fopen(filename.c_str(),"w");
    if(fp == nullptr){
        perror("Error");
    }

    for(int j = 0; j < 1000; j++){
        cout<<j<<endl;
        // complex<float>* buffer = fm_demod_out.pop()->data;
        double* buffer = fm_demod_left_out.pop()->data;
        for(int i = 0; i < CHUNK_SIZE; i++){
            complex<float> num = buffer[i];
            // fprintf(fp, "%f,%f\n", num.real(), num.imag());
            fprintf(fp, "%f\n", num.real());
        }
    }
    fclose(fp);

    return 0;
}


