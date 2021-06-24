#include <cstdio>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>	
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include "./utils/BlockingQueue.h"
#include "./utils/utils.h"
#include "./threads/capture_thread.h"
#include "./threads/stage_1_filtering_thread.h"
#include "./threads/FM_demod_thread.h"
#include "./threads/mono_audio_extraction_thread.h"
#include "./threads/pilot_extraction_thread.h"
#include "./threads/LR_diff_recovery_thread.h"
#include "./threads/LR_diff_extraction_thread.h"
#include "./threads/networking_thread.h"

int setup_socket(int port);

using namespace std;
using namespace chrono;

int CHUNK_SIZE = 512;
int Fs = 1.44e6;
int PORT = 4500;
bool tcp = true;
// int Fs = 2.4e6;

int main(int argc, char** argv){
	double fc = 88.7e6;
	if(argc < 2){
		printf("USAGE: main [center frequency]\n Default: 88.7e6\n");
	}else{
		fc = stod(argv[1]);
        printf("fs = %f\n", fc);
	}

    printf("Center Frequency: %.2fe6 Hz\n", fc/(1e6));

    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;
    BlockingQueue<complex<float>> capture_out;  // same as filter_in
    BlockingQueue<complex<float>> stage1_out;   // same as fm_demod_in
    BlockingQueue<double> fm_demod_out1;         // same as mono_audio_extraction_in
    BlockingQueue<double> fm_demod_out2;         // same as pilot_extraction_thread_in
    BlockingQueue<double> fm_demod_out3;         // same as LRdiff_extraction_thread_in
    BlockingQueue<double> mono_audio_extraction_out;

    // second path
    BlockingQueue<complex<double>> LR_diff_recovery_out;
    BlockingQueue<complex<double>> pilot_extraction_out1;
    BlockingQueue<complex<double>> pilot_extraction_out2;
    BlockingQueue<double> LR_diff_out;

    capture_args capture_config;
        capture_config.sample_rate = Fs;
        capture_config.center_freq = fc;
        // capture_config.center_freq = 93.7e6;
        capture_config.out = &capture_out;
        capture_config.chunk_size = CHUNK_SIZE;

    stage_1_filtering_args stage_1_config;
        stage_1_config.in = &capture_out;
        stage_1_config.out = &stage1_out;
        stage_1_config.ntaps = 12;
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
    
    pilot_extract_args_1 pilot_extract_config1;
        pilot_extract_config1.in = &fm_demod_out2;
        pilot_extract_config1.out = &pilot_extraction_out1;
        pilot_extract_config1.filter_path_diffeq_a = "./filters/18kHz_20kHz_bp_a1.txt";
        pilot_extract_config1.filter_path_diffeq_b = "./filters/18kHz_20kHz_bp_b1.txt";
        pilot_extract_config1.sample_rate = 480e3;
        pilot_extract_config1.chunk_size = CHUNK_SIZE;
        pilot_extract_config1.dec_rate = 5;
        pilot_extract_config1.taps = 64;

    pilot_extract_args_2 pilot_extract_config2;
        pilot_extract_config2.in = &pilot_extraction_out1;
        pilot_extract_config2.out = &pilot_extraction_out2;
        pilot_extract_config2.filter_path_diffeq_a = "./filters/18kHz_20kHz_bp_a2.txt";
        pilot_extract_config2.filter_path_diffeq_b = "./filters/18kHz_20kHz_bp_b2.txt";
        pilot_extract_config2.sample_rate = 480e3/5;
        pilot_extract_config2.chunk_size = CHUNK_SIZE;
        pilot_extract_config2.taps = 64;

    LR_diff_recovery_args LR_diff_config;
        LR_diff_config.in = &fm_demod_out3;
        LR_diff_config.out = &LR_diff_recovery_out;
        LR_diff_config.chunk_size = CHUNK_SIZE;
        LR_diff_config.dec_rate = 5;
        LR_diff_config.sample_rate = 480000;
        LR_diff_config.filter_path_diffeq_a = "./filters/22kHz_54kHz_bp_a.txt";
        LR_diff_config.filter_path_diffeq_b = "./filters/22kHz_54kHz_bp_b.txt";
    
    LR_diff_extract_args LR_diff_ext_config;
        LR_diff_ext_config.LR_diff = &LR_diff_recovery_out;
        LR_diff_ext_config.pilot = &pilot_extraction_out2;
        LR_diff_ext_config.out = &LR_diff_out;
        LR_diff_ext_config.chunk_size = CHUNK_SIZE;
        LR_diff_ext_config.dec_rate = 2;
        LR_diff_ext_config.sample_rate = 480000/5;
        LR_diff_ext_config.taps = 32;

        LR_diff_ext_config.filter_path_h = "./filters/LR_diff_filter_h_15kHz.txt";



    networking_args networking_config;
        networking_config.LRsum = &mono_audio_extraction_out;
        networking_config.LRdiff = &LR_diff_out;
        networking_config.chunk_size = CHUNK_SIZE;


    auto start_time = high_resolution_clock::now();
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

    pthread_t pilot_extraction_id1;
    pthread_create(&pilot_extraction_id1, NULL, &pilot_extraction_thread_stage_1_diffeq, &pilot_extract_config1);

    pthread_t pilot_extraction_id2;
    pthread_create(&pilot_extraction_id2, NULL, &pilot_extraction_thread_stage_2_diffeq, &pilot_extract_config2);

    pthread_t LR_diff_recovery_id;
    pthread_create(&LR_diff_recovery_id, NULL, &LR_diff_recovery_thread, &LR_diff_config);

    pthread_t LR_diff_extraction_id;
    pthread_create(&LR_diff_extraction_id, NULL, &LR_diff_extraction_thread, &LR_diff_ext_config);

    pthread_t networking_id;
    if(tcp == true){
        networking_config.socket_fd  = setup_socket(PORT);
        pthread_create(&networking_id, NULL, &networking_thread, &networking_config);
        pthread_join(networking_id, NULL);
    }

    pthread_join(capture_id, NULL);
    pthread_join(stage_1_id, NULL);
    pthread_join(fm_demod_id, NULL);
    pthread_join(mono_audio_extraction_id, NULL);
    pthread_join(pilot_extraction_id1, NULL);
    pthread_join(pilot_extraction_id2, NULL);
    pthread_join(LR_diff_recovery_id, NULL);
    pthread_join(LR_diff_extraction_id, NULL);

    // auto stop_time = high_resolution_clock::now();
    // auto duration = duration_cast<milliseconds>(stop_time-start_time);
    // cout<<"time spent: "<<duration.count()<<" ms"<<endl;

    return 0;
}

