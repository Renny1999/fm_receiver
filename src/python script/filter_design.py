import numpy as np
from scipy import signal

filename = "stage_1_filter.txt"
file = open(filename, 'w')

f_bw = 200000
n_taps = 64 
Fs = 1e6

lpf = signal.remez(64, [0, f_bw, f_bw+(Fs/2-f_bw)/4, Fs/2], [1,0], Hz=Fs)
lpf_fft = np.fft.fft(lpf, 512)

np.save("filter_output", lpf_fft)

for bin in lpf_fft:
	if(bin == 0):
		print(bin.real, bin.imag)
	print(bin)
	file.write("{}\n".format(bin.real))
	file.write("{}\n".format(bin.imag))
