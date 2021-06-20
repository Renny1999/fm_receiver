import numpy as np
from scipy import signal

def filter2file(filename, filter):
	file = open(filename, 'w')
	for i in range(0, len(filter)-1):
		coef = filter[i]
		file.write("{}\n".format(np.real(coef)))
		file.write("{}\n".format(np.imag(coef)))
	last_element = filter[len(filter)-1]
	file.write("{}\n".format(np.real(last_element)))
	file.write("{}".format(np.imag(last_element)))
	file.close()


filename1 = "./filters/stage_1_filter_fft_100kHz.txt"
filename2 = "./filters/stage_1_filter_h_100kHz.txt"

f_bw = 100000
n_taps = 8
Fs = 1.44e6

cutoff = f_bw
trans_width = 10000

# lpf = signal.remez(n_taps, [0, cutoff, cutoff+(Fs/2-f_bw), Fs/2], [1,0], Hz=Fs)
lpf = signal.remez(n_taps, [0, cutoff, cutoff+trans_width, Fs/2], [1,0], Hz=Fs)
lpf_fft = np.fft.fft(lpf*signal.windows.hann(n_taps), 512)

filter2file(filename1, lpf_fft)
filter2file(filename2, lpf)
