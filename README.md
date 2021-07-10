# fm_receiver
Dependencies:<br />
SoapySDR configured to use RTL-SDR <br />
FFTW3 with --enable-float

Need RTL-SDR module for SoapySDR <br />
1. librtlsdr `https://github.com/librtlsdr/librtlsdr <br />
2. https://github.com/pothosware/SoapyRTLSDR/wiki  <br />
3. https://github.com/pothosware/SoapySDR/wiki <br />

Compiling the Code: <br />
* `cd src` <br />
  `make`
* Flags needed before -o <br />
  * -pthread
* Flags needed after -o (for linking library)
  * -lSoapySDR  (needed to use SoapySDR)
  * -lfftw3f  (needed to use fftw3's float version)

![Screenshot 2021-06-26 202235](https://user-images.githubusercontent.com/33414341/123531794-6ca6d280-d6bc-11eb-9ad8-ad09891b33cd.png)
THREAD | INPUT | OUTPUT
---- | ---- | ----
capture thread | N/A | raw IQ sample <br /> `complex<float>`
stage 1 filtering thread | raw IQ sample <br /> `complex<float>` | decimated raw IQ sample that covers exactly one channel of FM radio <br />`complex<float>`
fm demod thread | decimated raw IQ sample that covers exactly one channel of FM radio <br /> `complex<float>` | FM-demodulated FM channel <br /> `double`
mono audio extraction thread | FM-demodulated FM channel <br/> `double` | L+R audio at around 44.1kHz <br/> `double`
pilot extraction thread | L-R audio with offset of 38kHzFM-demodulated FM channel <br/> `double` | 38kHz pilot signal achieved by multiplying the extracted 19kHz pilot signal by itself <br/> `complex<double>`
LRdiff recovery thread | FM-demodulated FM channel <br/> `double` | L-R audio with offset of 38kHz <br/> `complex<double>`
LRdiff extraction thread | L-R audio with offset of 38kHz <br/> `complex<double>` | L-R audio moved to the base band decimated to have Fs = 48kHz <br/> `double`
networking thread | L+R audio at 48kHz <br/> and <br/> L-R audio at 48kHz <br/> `double` | Sends the L and R channel to client through TCP <br/> `int16`



# TODOs: <br />
* [x] The dec_rate for mono audio extraction is 4.5, which was floored down to 4
  * This can cause audio to sound incorrect, so maybe perform interpolation then decimate?
  * SOLVED by using sample rates and decimation rates to perfectly achieve 48kHz sample frequency
* [ ] Complex number multiplication in stage_1_filtering_thread is more expensive than simply mulplying by a double
  * We know the coefficients in the butterworth filter difference equation are all real, so maybe take advantage of that?
* [ ] Take care of memory leaks
  * [x] Fix the memory leak caused by using the same pointer for each popped data
    * Solved by deleting the popped data at the end of every loop since it is not needed in the next thread which use a copy/new buffer
    * After such changes, it is observed that the program does not use unbounded amount of memory, indicating that perhaps nothing more need to be done in terms of memory management
    [ ] Minor memory leaks still exists, need investigation
* [ ] Verify stereo audio is working

# Notes: <br />
* ## Fs = 1.44MHz
  * First decimation: 1.44MHz / 3 = 480kHz
  * Second decimation: 480kHz / 10 = 48kHz
* ## stage 1 filtering thread was really slow, and could no keep up with the 1MHz+ sample rate
  * ### Three approaches were taken:
    1. [TOO SLOW] use the difference equation acquired from butterworth filter:
       * a<sub>0</sub>y[n] = b<sub>0</sub>x[n] + b<sub>1</sub>x[n-1] + b<sub>2</sub>x[n-2] + ... + b<sub>N</sub>x[n-N] - a<sub>1</sub>y[n-1] - b<sub>2</sub>y[n-2] + ... + a<sub>M</sub>y[n-M] <br />
       * This approach did not work mainly because the with a 3rd order butterworth filter, 8 multiplications for each sample was too slow.
       * Since the filtered signal is decimated, some y[n] are not used. however, the difference equation requires to know all y[n] values
       
    2. [NOISY] Filtering in Fourier domain: used fftw for FFT and iFFT:
       * The speed was decent, but the filtered signal ended up being fairly noisy due to the ripples caused by the FFT
    3. [SATISFACTORY] convolution with FIR filter impulse response:
       * y[n] = h[0]x[n] + h[1]x[n-1] + ... + h[j]x[n-j]
       * This method does not require any previous y value to calculate y[n]
       * The program can skip the convolution process on samples that will be dropped during decimated without causing any inconvenience
         * This approach only requires 1/dec_rate convolutions, making it even faster than the FFT approach
* ## Butterworth filters had better filter performance compared to the remez filters, but were slower to apply.
  * Originally, I used `std::deque<T>` to keep track to the past values for x and y, but the `[]` operator for `std::deque<T>` seemed to be slow
  * Since the difference equation method only need to accesss the past values for x and y incrementally, like `hist_x[0], hist_x[1], hist_x[2]...` where `hist_x[i] = x[n-i]`, the random access property in `std::deque<T>` was not necessary.
  * I made a `Deque<T>` class implemented using Linked List, and instead of using `deque[i]`, I prepared a `temp` pointer pointing at the first element of the `Deque`, and moved it to `temp->next` in each iteration of the loop.
  * This approach turned out to be faster than accessing past values using `deque[i]`, and made the use of butterworth filter difference equations viable in some occasions.
  * Now that I think about it, an iterator for `std::deque<T>` might have achieved the same effect.
* ## About memory leaks
  * ### `T* popped = BlockingQueue<T>::pop();` <br /> The memory address in `popped` is updated in every loop iteration, and the old address stored in `popped` will become unaccessible.
    * Because the data passed onto the next thread are allocated in a new buffer, the contents of the data stored in the memory address pointed to by `popped` will be no longer needed at the next iteration. Therefore, `popped` needed to be deleted at the end of each iteration.
  * ### Reuseable buffers
    * Many of the threads filters the data, decimates the filtered data, then passed the decimated data to the next thread.
    * The filter data will not be passed to the next thread, so they become useless after the decimation. 
    * Instead of allocating new buffer for the filtered data in each iteration, I used a reuseable buffer instead. I know when processing the data that goes into `sig_filtered[i]`, the data at `sig_filtered[i+1]~` are never accessed, so I can simply overwrite `sig_filtered[i]` to save me from the hassle of deleting the buffer and allocating a new one for the next iteration.
  * ### Buffers allocated in the middle of each loop 
    * _(May not be important since when the loop ends, the memory used by the program is freed anyway)_
    * Some buffers have to be allocated in the middle of the loop, such as the decimated data.
    * The loop might end abruptly before the newly allocated buffer is passed to the next thread because decimation desynchronizes the samples between each thread.
    * For all the buffers that are needed to be allocated inside the loop, I declared their pointers outside of the loop so I can clean the leftover data in the buffer outside the loop in case there are some residual data left in the buffer.


# Worth Noting: <br />
  * The filters used in the project are designed using scipy's signal library
  * Lowpass filters are designed using "remez", and bandpass filters are designed using "butter"
    * scipy's "butter" function might generate unstable filters when the order is high
                          
