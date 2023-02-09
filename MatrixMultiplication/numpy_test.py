import numpy as np
from time import perf_counter

if __name__ == '__main__':
    N = 64
    GIGA = 1e9
    GFLOP = N * N * N * 2 / GIGA

    ## Numpy
    A = np.random.uniform(size=(N, N)).astype(np.float32)
    B = np.random.uniform(size=(N, N)).astype(np.float32)


    for idx in range(4):
        time_init = perf_counter()
        C = A @ B
        time_final = perf_counter()

        GFLOPS = GFLOP / (time_final - time_init)

        if idx == 3:
            print(f"Numpy - GFLOP:      {GFLOP}")
            print(f"Numpy - GFLOPS:     {GFLOPS}\n")
    '''
    A = np.array(
            [[3, 7, 3, 6],
            [9, 2, 0, 3],
            [0, 2, 1, 7],
            [2, 2, 7, 9]]
            )

    B = np.array(
            [[6, 1, 6, 0],
            [1, 7, 6, 3],
            [6, 6, 8, 5],
            [0, 3, 5, 2]]
            )

    print(A)
    print(B)
    print(A @ B)
    '''
