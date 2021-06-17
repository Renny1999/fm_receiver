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

Fs = 1e6

b,a = signal.butter(5, 200000, btype='low', analog=False, fs=Fs, output='ba')

b_filename = "./filters/200kHz_lp_b.txt"
a_filename = "./filters/200kHz_lp_a.txt"
filter2file(b_filename, b)
filter2file(a_filename, a)

print(b)
print(a)
