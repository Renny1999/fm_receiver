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


filename = "./filters/stage_1_filter.txt"

f_bw = 200000
n_taps = 64 
Fs = 1e6

lpf = signal.remez(64, [0, f_bw, f_bw+(Fs/2-f_bw)/4, Fs/2], [1,0], Hz=Fs)
lpf_fft = np.fft.fft(lpf, 512)

filter2file(filename, lpf_fft)