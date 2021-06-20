# fm_receiver

Dependencies:<br />
SoapySDR configured to use RTL-SDR <br />
FFTW3 with --enable-float

need RTL-SDR module for SoapySDR <br />
https://github.com/pothosware/SoapyRTLSDR/wiki  <br />
https://github.com/pothosware/SoapySDR/wiki <br />


Compiling the Code: <br />
* Flags needed before -o <br />
  * -pthread
* Flags needed after -o (for linking library)
  * -lSoapySDR  (needed to use SoapySDR)
  * -lfftw3f  (needed to use fftw3's float version)

TODOs: <br />
* [SOLVED] The dec_rate for mono audio extraction is 4.5, which was floored down to 4
  * this can cause audio to sound incorrect, so maybe perform interpolation then decimate?
  * SOLVED by using sample rates and decimation rates to perfectly achieve 48kHz sample frequency
* [?] complex number multiplication in stage_1_filtering_thread is more expensive than simply mulplying by a double
  * we know the coefficients in the butterworth filter difference equation are all real, so maybe take advantage of that?

Notes: <br />
* Fs = 1.44MHz
  * first decimation: 1.44MHz / 3 = 480kHz
  * second decimation: 480kHz / 10 = 48kHz
* stage 1 filtering thread was really slow, and could no keep up with the 1MHz+ sample rate
  * Three approaches were taken:
    1. [TOO SLOW] use the difference equation acquired from butterworth filter:
       * a<sub>0</sub>y[n] = b<sub>0</sub>x[n] + b<sub>1</sub>x[n-1] + b<sub>2</sub>x[n-2] + ... + b<sub>N</sub>x[n-N] - a<sub>1</sub>y[n-1] - b<sub>2</sub>y[n-2] + ... + a<sub>M</sub>y[n-M] <br />
       * this approach did not work mainly because the with a 3rd order butterworth filter, 8 multiplications for each sample was too slow.
       * since the filtered signal is decimated, some y[n] are not used. however, the difference equation requires to know all y[n] values
       
    2. [NOISY] filtering in Fourier domain: used fftw for FFT and iFFT:
       * the speed was decent, but the filtered signal ended up being fairly noisy due to the ripples caused by the FFT
    3. [SATISFACTORY] convolution with FIR filter impulse response:
       * y[n] = h[0]x[n] + h[1]x[n-1] + ... + h[j]x[n-j]
       * this method does not require any previous y value to calculate y[n]
       * the program can skip the convolution process on samples that will be dropped during decimated without causing any inconvenience
         * this approach only requires 1/dec_rate convolutions, making it even faster than the FFT approach
                          
