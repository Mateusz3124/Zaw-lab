from multiprocessing.managers import BaseManager
from multiprocessing import Array, Pool

class QueueManager(BaseManager):
    pass

def f(matrix_index, matrix_rows, vec, j, dif):
    count = 0
    QueueManager.register('out_queue')
    m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
    m.connect()
    out_queue = m.out_queue()
    for matrix_row in matrix_rows:
        sum = 0.0

        for i in range(len(matrix_row)):
            sum += matrix_row[i] * vec[0][i]
        
        out_queue.put_nowait((matrix_index,j * dif + count,sum))
        count += 1

QueueManager.register('in_queue')
m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
m.connect()
in_queue = m.in_queue()

with Pool(processes=30) as pool:
    counter = 0
    while True:
        data = in_queue.get()
        dif = int(len(data[1]) / 9)
        if dif != 0:
            for i in range(9):
                pool.apply_async(f, (data[0], data[1][i * dif : i * dif + dif], data[2], i, dif))
        pool.apply_async(f, (data[0], data[1][9 * dif : len(data[1])], data[2], 9, dif))