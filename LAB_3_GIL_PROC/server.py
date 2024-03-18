from multiprocessing.managers import BaseManager
import queue

class QueueManager(BaseManager):
    pass

queue = queue.Queue()
QueueManager.register('get_queue', callable=lambda:queue)
m = QueueManager(address=('127.0.0.1', 8888), authkey=bytes('abracadabra', encoding='utf-8'))
s = m.get_server()
s.serve_forever()

def main(ip, port):
    