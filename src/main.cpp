#include <cstdio>
#include <cstdlib>
#include <vector>
#include <chrono>
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
#include "./threads/mono_audio_extraction_thread.h"


using namespace std;
using namespace chrono;

int CHUNK_SIZE = 512;
int Fs = 1.44e6;
// int Fs = 2.4e6;

int main(){

    auto start_time = high_resolution_clock::now();

    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;
    BlockingQueue<complex<float>> capture_out;  // same as filter_in
    BlockingQueue<complex<float>> stage1_out;   // same as fm_demod_in
    BlockingQueue<double> fm_demod_out1;         // same as mono_audio_extraction_in
    BlockingQueue<double> fm_demod_out2;         // same as pilot_extraction_thread_in
    BlockingQueue<double> fm_demod_out3;         // same as LRdiff_extraction_thread_in
    BlockingQueue<double> mono_audio_extraction_out;

    capture_args capture_config;
        capture_config.sample_rate = Fs;
        capture_config.center_freq = 88.7e6;
        capture_config.out = &capture_out;
        capture_config.chunk_size = CHUNK_SIZE;

    stage_1_filtering_args stage_1_config;
        stage_1_config.in = &capture_out;
        stage_1_config.out = &stage1_out;
        stage_1_config.ntaps = 8;
        stage_1_config.filter_path_fft = "./filters/stage_1_filter_fft_100kHz.txt";
        stage_1_config.filter_path_h = "./filters/stage_1_filter_h_100kHz.txt";
        stage_1_config.filter_path_diffeq_a = "./filters/100kHz_lp_a.txt";
        stage_1_config.filter_path_diffeq_b = "./filters/100kHz_lp_b.txt";
        stage_1_config.signal_bw = 200*1000;    // 200kHz
        stage_1_config.dec_rate = 3;
        stage_1_config.sample_rate = Fs;
        stage_1_config.chunk_size = CHUNK_SIZE;

    

    FM_demod_args fm_demod_config;
        fm_demod_config.in = &stage1_out;
        fm_demod_config.out1 = &fm_demod_out1;
        fm_demod_config.out2 = &fm_demod_out2;
        fm_demod_config.out3 = &fm_demod_out3;
        fm_demod_config.chunK_size = CHUNK_SIZE;
        fm_demod_config.sample_rate = 480e3;   

    m_audio_extract_args m_audio_extract_config;
        m_audio_extract_config.in = &fm_demod_out1;
        m_audio_extract_config.out = &mono_audio_extraction_out;
        m_audio_extract_config.filter_path_fft = "./filters/stage_1_filter.txt";
        m_audio_extract_config.filter_path_diffeq_a = "./filters/15kHz_lp_a.txt";
        m_audio_extract_config.filter_path_diffeq_b = "./filters/15kHz_lp_b.txt";
        m_audio_extract_config.signal_bw = 44.1*1000;    
        m_audio_extract_config.sample_rate = 480e3;
        m_audio_extract_config.dec_rate = 10;
        m_audio_extract_config.chunk_size = CHUNK_SIZE;
      
    pthread_t capture_id;
    pthread_create(&capture_id, NULL, &capture_thread, &capture_config);

    pthread_t stage_1_id;
    // pthread_create(&stage_1_id, NULL, &stage_1_filtering_thread_diffeq_ll, &stage_1_config);
    // pthread_create(&stage_1_id, NULL, &stage_1_filtering_thread_diffeq_dq, &stage_1_config);
    // pthread_create(&stage_1_id, NULL, &stage_1_filtering_thread_fft, &stage_1_config);
    pthread_create(&stage_1_id, NULL, &stage_1_filtering_thread_h, &stage_1_config);

    pthread_t fm_demod_id;
    pthread_create(&fm_demod_id, NULL, &FM_demod_thread, &fm_demod_config);

    pthread_t mono_audio_extraction_id;
    pthread_create(&mono_audio_extraction_id, NULL, &mono_audio_extraction_thread_diffeq, &m_audio_extract_config);


    pthread_join(capture_id, NULL);
    pthread_join(stage_1_id, NULL);
    pthread_join(fm_demod_id, NULL);
    pthread_join(mono_audio_extraction_id, NULL);

    FILE *fp;
    // string filename = "./output/filtered_result.txt";
    string filename = "./output/exp/audio.txt";
    fp = fopen(filename.c_str(),"w");
    if(fp == nullptr){
        perror("Error");
    }

    auto stop_time = high_resolution_clock::now();
    cout<<"[Main]   done waiting"<<endl;
    for(int j = 0; j < 1000*8; j++){
        cout<<j<<endl;
        // complex<float>* buffer = stage1_out.pop(3000)->data;
        QueueElement<double>* e = mono_audio_extraction_out.pop(3000);
        if(e == nullptr){
            cout<<"timed out and no more data"<<endl;
            break;
        }
        double* buffer = e->data;
        for(int i = 0; i < CHUNK_SIZE; i++){
            complex<float> num = buffer[i];
            fprintf(fp, "%f,%f\n", num.real(), num.imag());
            // fprintf(fp, "%f\n", num.real());
        }
    }
    fclose(fp);
    cout<<"saved"<<endl;
    auto duration = duration_cast<microseconds>(stop_time-start_time);
    cout<<"time spent: "<<duration.count()<<" us"<<endl;

    return 0;
}


