from multiprocessing.managers import BaseManager

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

def divide_work(num_work, num_procs):
	size_of_division = int(num_work / num_procs)
	rest = num_work - num_procs * size_of_division

	divisions = [size_of_division for _ in range(num_procs)]
	for i in range(int(rest)):
		divisions[i] += 1 

	return divisions

def main(ip, port, num_procs):
	try:
		mat = read("./A.dat")
		vec = read("./X.dat")
	except:
		raise ValueError("coudn't read matrixes")

	QueueManager.register('in_queue')
	QueueManager.register('out_queue')
	manager = QueueManager(address=(ip, int(port)), authkey=bytes('abracadabra', encoding='utf-8'))
	manager.connect()
	
	in_queue = manager.in_queue()
	out_queue = manager.out_queue()

	num_procs = int(num_procs)
	num_rows = len(vec[0])

	if num_rows != len(mat[0]):
		raise ValueError("matrix sizes are incorrect")

	if num_procs > num_rows:
		raise ValueError("there are too many processes for the matrix")

	divisions = divide_work(len(mat), num_procs)

	sum = 0
	for i in range(len(divisions)):
		in_queue.put((i, mat[sum:sum+divisions[i]],vec))
		sum += divisions[i]

	for i in range(1, len(divisions)):
		divisions[i] += divisions[i-1]

	result = [None] * len(mat)
	for i in range(0, len(mat)):
		value = out_queue.get()
		if value[0] == 0:
			result[value[1]] = value[2]
			continue
		idx = divisions[value[0] - 1] + value[1]
		result[idx] = value[2]

	print("[")
	for i in range(len(mat)):
		print(str(result[i]) + ", ")
	print("]")

if __name__ == '__main__':
	main('127.0.0.1', 8888, 3)