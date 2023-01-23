try:
    import GBDT
    import numpy as np
    import pandas as pd


    mod = GBDT.GBM(
            -1,
             0.00,
             0.10,
             1.00,
             20,
             50,
             255,
             127,
             0.80,
             True
             )
    X = np.random.randint(0, 100, size=(100, 100))
    y = np.random.randint(0, 100, size=(100, 1))
    mod.train_hist(
            X=X,
            y=y
            )


    print('GBDT PASSED')
except Exception as E:
    print('GBDT FAILED')
    print(f'Error: {E}')


