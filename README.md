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
* The dec_rate for mono audio extraction is 4.5, which was floored down to 4
  * this can cause audio to sound incorrect, so maybe perform interpolation then decimate?
* complex number multiplication in stage_1_filtering_thread is more expensive than simply mulplying by a double
  * we know the coefficients in the butterworth filter difference equation are all real, so maybe take advantage of that?
