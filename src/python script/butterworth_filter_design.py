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

Fs = 1.44e6

# stage 1 filtering
b,a = signal.butter(2, 100000, btype='low', analog=False, fs=Fs, output='ba')

b_filename = "./filters/100kHz_lp_b.txt"
a_filename = "./filters/100kHz_lp_a.txt"
filter2file(b_filename, b)
filter2file(a_filename, a)

Fs = 480000

# mono audio extract
b,a = signal.butter(4, 15000, btype='low', analog=False, fs=Fs, output='ba')
b_filename = "./filters/15kHz_lp_b.txt"
a_filename = "./filters/15kHz_lp_a.txt"
filter2file(b_filename, b)
filter2file(a_filename, a)

# pilot extract 1
b_filename = "./filters/18kHz_20kHz_bp_b.txt"
a_filename = "./filters/18kHz_20kHz_bp_a.txt"
b,a = signal.butter(3, [17.5e3, 20.5e3], btype='band', fs=Fs)
filter2file(b_filename, b)
filter2file(a_filename, a)

Fs = 480000/5
# pilot_extract 2
b_filename = "./filters/37kHz_39kHz_bp_b.txt"
a_filename = "./filters/37kHz_39kHz_bp_a.txt"
b,a = signal.butter(4, [37e3, 39e3], btype='band', fs=Fs)
filter2file(b_filename, b)
filter2file(a_filename, a)


Fs = 480000
# diff recovery
b_filename = "./filters/22kHz_54kHz_bp_b.txt"
a_filename = "./filters/22kHz_54kHz_bp_a.txt"
b,a = signal.butter(5, [21e3, 55e3], btype='band', fs=Fs)
filter2file(b_filename, b)
filter2file(a_filename, a)
