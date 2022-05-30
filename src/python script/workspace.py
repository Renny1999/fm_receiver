import numpy as np

def readfiles(count: np.uint16):
	"""
	reads data files into numpy array
	"""
	nums=np.arange(0,count+1)
	darr=[]
	for n in nums:
		fname = '../output/{}.data'.format(n)
		f = open(fname, 'r')
		for line in f:
			splitted=line.split(' ')
			realpart =np.float32(splitted[0])
			imagpart =np.float32(splitted[1])
			darr.append(realpart + 1j*imagpart)
		
	return np.asarray(darr)

