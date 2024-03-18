from multiprocessing.managers import BaseManager
from multiprocessing import Array, Pool
import time

class QueueManager(BaseManager):
    pass

def f(value, vec):
    matrix_index = value[0]
    matrix_row = value[1]
    QueueManager.register('out_queue')
    m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
    m.connect()
    queue = m.out_queue()
    sum = 0.0
    for i in range(len(matrix_row)):
        sum += matrix_row[i] * vec[i]
    queue.put((matrix_index,sum))

QueueManager.register('in_queue')
m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
m.connect()
queue = m.in_queue()
vec_given = queue.get()
num_rows = queue.get()
vec = Array('f', range(len(vec_given)))
vec = vec_given
with Pool(processes=4) as pool:
    for i in range(num_rows):
        queue_value = queue.get()
        pool.apply_async(f, (queue_value,vec))
    pool.close()
    pool.join()
