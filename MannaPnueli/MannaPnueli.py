import threading
from time import sleep
from random import random

NUM_THREADS = 4
ITERATIONS = 100

SUM = 0

REQUEST = 0
RESPOND = 0


def server():
    global REQUEST
    global RESPOND
    global ITERATIONS

    for _ in range(ITERATIONS * (NUM_THREADS-1)):
        while REQUEST == 0:
            continue
        RESPOND = REQUEST
        while RESPOND != 0:
            continue
        REQUEST = 0


def client():
    global REQUEST
    global RESPOND
    global SUM
    global ITERATIONS

    local = threading.local()
    local.id = threading.get_ident()

    for _ in range(ITERATIONS):
        # pre-protocol
        while RESPOND != local.id:
            REQUEST = local.id

        # critical section
        local.sum = SUM
        # sleep(random())
        SUM = local.sum + 1

        # end of critical section
        RESPOND = 0


def main():
    threads = list()

    threads.append(threading.Thread(target=server))
    threads[0].start()

    for i in range(1, NUM_THREADS):
        threads.append(threading.Thread(target=client))
        threads[i].start()

    for thread in threads:
        thread.join()

    print(SUM)


if __name__ == "__main__":
    main()
