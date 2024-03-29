from multiprocessing.managers import BaseManager
from multiprocessing import Array, Pool

class QueueManager(BaseManager):
    pass

def f(matrix_index, matrix_row, vec, j):
    sum = 0.0
    m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
    m.connect()
    out_queue = m.out_queue()

    for i in range(len(matrix_row)):
        sum += matrix_row[i] * vec[0][i]
    
    out_queue.put((matrix_index,j,sum))

def main():
    QueueManager.register('in_queue')
    QueueManager.register('out_queue')
    m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
    m.connect()
    in_queue = m.in_queue()

    with Pool(processes=10) as pool:
        while True:
            data = in_queue.get()

            for i in range(len(data[1])):
                pool.apply_async(f, (data[0], data[1][i], data[2], i))
            
        pool.close()
        pool.join()

if __name__ == '__main__':
	main()
