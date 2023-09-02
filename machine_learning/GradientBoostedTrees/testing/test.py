from GBDT import GBDT
import lightgbm as lgb
import pandas as pd
import numpy as np
import os


if __name__ == '__main__':
    model = GBDT(
            num_boosting_rounds=10_000,
            max_leaves=31, 
            max_depth=5,
            min_data_in_leaf=20, 
            col_subsample_rate=0.2,
            max_bin=255,
            lr=0.1,
            dart=False,
            verbosity=50,
            early_stopping_steps=50_000,
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

    model.train_eval(df)
