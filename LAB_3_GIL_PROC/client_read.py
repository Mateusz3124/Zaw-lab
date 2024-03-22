from multiprocessing.managers import BaseManager
import sys

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

def main(ip, port, num_procs):
	num_procs = int(num_procs)
	try:
		mat = read("./A.dat")
		vec = read("./X.dat")
	except:
		raise ValueError("coudn't read matrixes")
	QueueManager.register('in_queue')
	QueueManager.register('out_queue')
	manager = QueueManager(address=(ip, int(port)), authkey=bytes('abracadabra', encoding='utf-8'))
	manager.connect()
	queue = manager.in_queue()

	num_rows = len(vec[0])

	if len(vec[0]) != len(mat):
		raise ValueError("matrix sizes are incorrect")

	if num_procs > num_rows:
		raise ValueError("there are too many processes for the matrix")

	size_of_division = int(len(mat) / num_procs)
	divisions = []
	for i in range(num_procs):
		divisions.append(size_of_division)
	rest = len(mat) - num_procs * size_of_division;
	for i in range(int(rest)):
		divisions[i] += 1 
	
	sum = 0

	for i in range(len(divisions)):
		queue.put((i, mat[sum:sum+divisions[i]],vec))
		sum += divisions[i]

	queue_result = manager.out_queue()

	result = [None] * num_rows
	
	for i in range(1, len(divisions)):
		divisions[i] += divisions[i-1]

	for i in range(0, num_rows):
		value = queue_result.get()
		if value[0] == 0:
			result[value[1]] = value[2]
			continue
		idx = divisions[value[0] - 1] + value[1]
		result[idx] = value[2]

	print("[")
	for i in range(num_rows):
		print(str(result[i]) + ", ")
	print("]")

if __name__ == '__main__':
    main(*sys.argv[1:])