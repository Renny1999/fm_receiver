# fm_radio_receiver

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
