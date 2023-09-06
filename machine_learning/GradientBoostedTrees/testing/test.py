from GBDT import GBDT
import pandas as pd
import numpy as np
import os
from time import perf_counter

if __name__ == '__main__':
    SUBSAMPLE_RATE = 0.5
    MAX_LEAVES = 31
    DEPTH = 100

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

    X = np.random.rand(1_000_000, 50).astype(np.float32)
    y = np.random.rand(1_000_000).astype(np.float32)
    df = pd.DataFrame(X)
    df['target'] = y

    init = perf_counter()
    model.train_eval(df)
    gbdt_time = perf_counter() - init

    import lightgbm as lgb
    from sklearn.ensemble import HistGradientBoostingRegressor

    init = perf_counter()
    lgb.train(
            params={
                'objective': 'regression',
                'num_leaves': MAX_LEAVES,
                'max_depth': DEPTH,
                'min_data_in_leaf': 20,
                'feature_fraction': SUBSAMPLE_RATE,
                'sample_for_bin': 1_000_000,
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
    lgb_time = perf_counter() - init


    init = perf_counter()
    model = HistGradientBoostingRegressor(
            max_iter=1_000,
            max_leaf_nodes=MAX_LEAVES,
            max_depth=DEPTH,
            min_samples_leaf=20,
            learning_rate=0.1,
            verbose=50,
            validation_fraction=0.0001,
            n_iter_no_change=5_000
            )
    model.fit(X, y)
    sklearn_time = perf_counter() - init

    
    print(f'LightGBM: {lgb_time:.2f}s')
    print(f'GBDT:     {gbdt_time:.2f}s')
    print(f'Sklearn:  {sklearn_time:.2f}s')
