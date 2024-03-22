from multiprocessing.managers import BaseManager
from multiprocessing import Array, Pool

class QueueManager(BaseManager):
    pass

def f(matrix_index, matrix_row, vec, j):

    QueueManager.register('out_queue')
    m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
    m.connect()
    queue = m.out_queue()
    sum = 0.0

    for i in range(len(matrix_row)):
        sum += matrix_row[i] * vec[0][i]
    
    queue.put((matrix_index,j,sum))

QueueManager.register('in_queue')
m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
m.connect()
queue = m.in_queue()

while True:
    data = queue.get()
    with Pool(processes=10) as pool:

        for i in range(len(data[1])):
            pool.apply_async(f, (data[0],data[1][i],data[2], i))
        
        pool.close()
        pool.join()
