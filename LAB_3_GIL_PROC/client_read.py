from multiprocessing.managers import BaseManager
import math, sys, time
import queue

def read(fname):
	f = open(fname, "r")
	nr = int(f.readline())
	nc = int(f.readline())

	A = [[0] * nc for x in range(nr)]
	r = 0
	c = 0
	for i in range(0,nr*nc):
		A[r][c] = float(f.readline())
		c += 1
		if c == nc:
			c = 0
			r += 1

	return A


class QueueManager(BaseManager): 
    pass

def main(ip, port):
	mat = read("./A.dat")
	vec = read("./X.dat")
	QueueManager.register('in_queue')
	manager = QueueManager(address=(ip, int(port)), authkey=bytes('abracadabra', encoding='utf-8'))
	manager.connect()
	queue = manager.in_queue()
	# (indeks_macierzy, wiersz macierzy, wektor)

	queue.put((0, mat[0], vec))
	queue.put((1, mat[1], vec))

	# for row_index in range(0, len(vec)+1):
	# 	print(mat[row_index])
	# 	queue.put((row_index, mat[row_index], vec))

if __name__ == '__main__':
    main("127.0.0.1", 8888)