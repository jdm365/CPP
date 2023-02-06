import numpy as np
import random
from time import perf_counter
import mkl

mkl.set_num_threads(1)

def matmult(a, b):
    zip_b = zip(*b)
    # uncomment next line if python 3 : 
    # zip_b = list(zip_b)
    return [[sum(ele_a*ele_b for ele_a, ele_b in zip(row_a, col_b)) 
             for col_b in zip_b] for row_a in a]

if __name__ == '__main__':
    '''
    N = 512 
    GIGA = 1e9
    GFLOP = N * N * N * 2 / GIGA

    ## Pure python
    time_init = perf_counter()
    A = [[random.uniform(0, 10) for _ in range(N)] for _ in range(N)]
    B = [[random.uniform(0, 10) for _ in range(N)] for _ in range(N)]
    C = matmult(A, B)
    time_final = perf_counter()

    GFLOPS = GFLOP / (time_final - time_init)

    print(f"Python - GFLOP:      {GFLOP}")
    print(f"Python - GFLOPS:     {GFLOPS}\n")


    ## Numpy
    A = np.random.uniform(size=(N, N)).astype(np.float32)
    B = np.random.uniform(size=(N, N)).astype(np.float32)


    for idx in range(4):
        time_init = perf_counter()
        C = A @ B
        time_final = perf_counter()

        GFLOPS = GFLOP / (time_final - time_init)

        if idx > 1:
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
