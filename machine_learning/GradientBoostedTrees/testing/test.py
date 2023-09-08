from GBDT import GBDT
import pandas as pd
import os
from time import perf_counter

if __name__ == '__main__':
    ## SUBSAMPLE_RATE = 0.2
    SUBSAMPLE_RATE = 1.0
    MAX_LEAVES = 31
    DEPTH = 5
    NUM_BOOSTING_ROUNDS = 500

    model = GBDT(
            num_boosting_rounds=NUM_BOOSTING_ROUNDS,
            max_leaves=MAX_LEAVES,
            max_depth=DEPTH,
            min_data_in_leaf=20,
            col_subsample_rate=SUBSAMPLE_RATE,
            max_bin=255,
            lr=0.1,
            dart=False,
            verbosity=25,
            early_stopping_steps=5_000,
            gpu=False
            )

    #FILEPATH = '../data/iris_dataset.csv'
    #FILEPATH = '../data/housing_price_prediction_dataset.csv'
    FILEPATH = '../data/HIGGS.feather'

    FILEPATH = os.path.join(os.path.dirname(__file__), FILEPATH)
    ## df = pd.read_csv(FILEPATH)
    df = pd.read_feather(FILEPATH)

    ## X, y = df.iloc[:, :-1], df.iloc[:, -1]
    y = df['target'].values
    X = df.drop('target', axis=1).values

    df = pd.DataFrame(X)
    df['target'] = y

    init = perf_counter()
    model.train_eval(df)
    gbdt_time = perf_counter() - init

    import lightgbm as lgb
    from sklearn.ensemble import HistGradientBoostingRegressor

    init = perf_counter()
    params = {
        'objective': 'regression',
        'num_leaves': MAX_LEAVES,
        'max_depth': DEPTH,
        'min_data_in_leaf': 20,
        'feature_fraction': SUBSAMPLE_RATE,
        'sample_for_bin': 1_000_000,
        'max_bin': 255,
        'learning_rate': 0.1,
        'verbosity': -1,
        'min_gain_to_split': 0.0,
        'lambda_l1': 0.0,
        'lambda_l2': 0.0,
        'min_sum_hessian_in_leaf': 0.0,
        'early_stopping_rounds': 5_000,
        'num_boost_round': NUM_BOOSTING_ROUNDS,
        'boosting': 'gbdt',
        'verbose_eval': 50
    }
    dataset = lgb.Dataset(X, y)
    lgb.train(params, dataset, valid_sets=[dataset], verbose_eval=50)
    lgb_time = perf_counter() - init


    init = perf_counter()
    model = HistGradientBoostingRegressor(
            max_iter=NUM_BOOSTING_ROUNDS,
            max_leaf_nodes=MAX_LEAVES,
            max_depth=DEPTH,
            min_samples_leaf=20,
            learning_rate=0.1,
            verbose=25,
            validation_fraction=0.0001,
            n_iter_no_change=5_000
            )
    model.fit(X, y)
    sklearn_time = perf_counter() - init

    
    print(f'LightGBM: {lgb_time:.2f}s')
    print(f'GBDT:     {gbdt_time:.2f}s')
    print(f'Sklearn:  {sklearn_time:.2f}s')
