#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h> // sleep()
#include <string.h>

#include <fstream>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include "capture_thread.h"

using namespace std;

void* capture_thread(void* args){
    sleep(1);
    capture_args* capture_config = (capture_args*) args;
    BlockingQueue<complex<float>>* out = capture_config->out;
    int CHUNK_SIZE = capture_config->chunk_size;

    SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
    SoapySDR::Kwargs::iterator it;

    for (int i = 0; i < results.size(); i++){
        printf("Found device #%d: ", i);

        for(it = results[i].begin(); it != results[i].end(); it++){
            printf("%s = %s\n", it->first.c_str(), it->second.c_str());
        }
        printf("\n");
    }

    // 1. create device instance

    //      1.1 set arguments
    //          args can be user defined or from the enumeration result
    //          we use first results as args here:
    SoapySDR::Kwargs radio_args = results[0];

    //      1.2 make device
    SoapySDR::Device *sdr = SoapySDR::Device::make(radio_args);

    if(sdr == nullptr){
        fprintf(stderr, "SoapySDR::Device::make failed\n");
    }

    // 2. query device info
    vector<string> str_list;    

    //      2.1 antennas
    str_list = sdr->listAntennas(SOAPY_SDR_RX, 0);
    printf("Rx antennas: ");
    for(int i = 0; i < str_list.size(); i++){
        printf("%s,", str_list[i].c_str());
    }
    printf("\n");

    //      2.2 gains
    str_list = sdr->listGains(SOAPY_SDR_RX, 0);
    printf("Rx Gains: ");
    for(string str : str_list){
        printf("%s, ", str.c_str());
    }
    printf("\n");

    // 3. apply settings
    sdr->setSampleRate(SOAPY_SDR_RX, 0, capture_config->sample_rate);
    sdr->setFrequency(SOAPY_SDR_RX, 0, capture_config->center_freq);

    // 4. setup a stream (complex floats)
    SoapySDR::Stream *rx_stream = sdr->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32);
    if(rx_stream == NULL){
        fprintf(stderr, "Failed to start a stream\n");
        SoapySDR::Device::unmake(sdr);
    }

    sdr->activateStream(rx_stream, 0, 0, 0);

    // 5. create a re-usable buffer for rx samples

    // 6. receive some samples
    // while(true){

    FILE* fp;
    fp = fopen("output/exp/unfiltered_1M.txt", "w");
    for(int i = 0; i < 5000; i++){
        complex<float>* data = new complex<float>[CHUNK_SIZE];
        void* buffs[] = {data};
        int flags;
        long long time_ns;
        int ret = sdr->readStream(rx_stream, buffs, CHUNK_SIZE, flags, time_ns, 1e5);

        for(int i = 0; i < CHUNK_SIZE; i++){
            complex<float> sum = data[i];
            fprintf(fp, "%f,%f\n", sum.real(), sum.imag());
        }
        out->push((complex<float>*) data);
    }

    fclose(fp);

    return nullptr;

}