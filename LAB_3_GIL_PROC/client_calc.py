from multiprocessing.managers import BaseManager

class QueueManager(BaseManager):
    pass

QueueManager.register('in_queue')
QueueManager.register('out_queue')
m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
m.connect()
queue = m.in_queue()
print(queue.get())
print(queue.get())
