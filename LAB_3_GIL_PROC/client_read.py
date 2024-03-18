from multiprocessing.managers import BaseManager
import time
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
	QueueManager.register('out_queue')
	manager = QueueManager(address=(ip, int(port)), authkey=bytes('abracadabra', encoding='utf-8'))
	manager.connect()
	queue = manager.in_queue()

	num_rows = len(vec[0])

	queue.put(vec[0])
	queue.put(num_rows)

	for row_index in range(0, num_rows):
		queue.put((row_index, mat[row_index]))

	queue_result = manager.out_queue()

	result = [None] * num_rows

	for row_index in range(0, num_rows):
		value = queue_result.get()
		result[value[0]] = value[1]
	print("[")
	for i in range(num_rows):
		print(str(result[i]) + ", ")
	print("]")
if __name__ == '__main__':
    main("127.0.0.1", 8888)