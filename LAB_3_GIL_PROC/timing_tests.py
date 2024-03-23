from client_read import read_and_get_answers, read
import time

def from_row_to_col_major_vec(row_vec):
	col_vec = [[x[0] for x in row_vec]]
	return col_vec

# requires my_server and client_calc to be running
def main(ip, port):
	mat = read("./BIG_A.dat")
	vec = read("./BIG_X.dat")
	vec = from_row_to_col_major_vec(vec)

	for i in range(2, len(mat)-1, int(len(vec[0])/10)):
		num_procs = i	

		if len(vec[0]) != len(mat[0]):
			raise ValueError("matrix sizes are incorrect")

		if num_procs > len(vec[0]):
			raise ValueError("there are too many processes for the matrix")

		t1 = time.time()
		read_and_get_answers(ip, port, num_procs, mat, vec)
		t2 = time.time()
		print(t2 - t1)

if __name__ == '__main__':
	main('127.0.0.1', 8888)