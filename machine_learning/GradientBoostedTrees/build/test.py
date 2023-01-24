try:
    import GBDT
    import numpy as np
    import pandas as pd

    df = pd.read_csv('data/hpx10.csv')
    X  = df[df.columns[:-1]].to_numpy().astype(np.float32)
    y  = df[df.columns[-1]].to_numpy().astype(np.float32)

    mod = GBDT.GBM(
            -1,
             0.00,
             0.50,
             1.00,
             20,
             100,
             255,
             127,
             0.80,
             True 
             )
    mod.train_hist(X=X, y=y)


    print('GBDT PASSED')
except Exception as E:
    print('GBDT FAILED')
    print(f'Error: {E}')


