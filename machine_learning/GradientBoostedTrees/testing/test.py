from GBDT import GBDT
import lightgbm as lgb
import pandas as pd
import numpy as np
import os
from time import perf_counter


if __name__ == '__main__':
    SUBSAMPLE_RATE = 0.1
    MAX_LEAVES = 31
    DEPTH = -1

    model = GBDT(
            num_boosting_rounds=1_000,
            max_leaves=MAX_LEAVES,
            max_depth=DEPTH,
            min_data_in_leaf=20, 
            col_subsample_rate=SUBSAMPLE_RATE,
            max_bin=255,
            lr=0.1,
            dart=False,
            verbosity=50,
            early_stopping_steps=5_000,
            gpu=False
            )

    #FILEPATH = '../data/iris_dataset.csv'
    FILEPATH = '../data/housing_price_prediction_dataset.csv'
    #FILEPATH = '../data/hpx10.csv'
    #FILEPATH = '../data/hpx100.csv'
    #FILEPATH = '../data/preprocessed_train.feather'

    FILEPATH = os.path.join(os.path.dirname(__file__), FILEPATH)
    df = pd.read_csv(FILEPATH)

    X, y = df.iloc[:, :-1], df.iloc[:, -1]

    import logging
    logger = logging.getLogger('lightgbm')
    logger.setLevel(logging.ERROR)

    X = np.random.rand(1_000_000, 100)
    y = np.random.rand(1_000_000)
    df = pd.DataFrame(X)
    df['target'] = y

    init = perf_counter()
    lgb.train(
            params={
                'objective': 'regression',
                'num_leaves': MAX_LEAVES,
                'max_depth': DEPTH,
                'min_data_in_leaf': 20,
                'feature_fraction': SUBSAMPLE_RATE,
                'max_bin': 255,
                'learning_rate': 0.1,
                'verbosity': -1,
                'early_stopping_rounds': 5_000,
                'num_boost_round': 1_000,
                'boosting': 'gbdt'
                },
            train_set=lgb.Dataset(X, y),
            valid_sets=[lgb.Dataset(X, y)]
            )
    print(f'LightGBM: {perf_counter() - init:.2f} seconds')


    init = perf_counter()
    model.train_eval(df)
    print(f'GBDT: {perf_counter() - init:.2f} seconds')

