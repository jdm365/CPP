import pandas as pd
import lightgbm as lgb



df = pd.read_csv('../data/housing_price_prediction_dataset.csv', header=None)
params = {
        'num_threads': 1,
        'max_depth': 6,
        'max_leaves': 1000,
        'is_enable_sparse': 'true',
        'enable_bundle': 'true',
        }
train_cols = df.columns[:-1]
target     = df.columns[-1]
train_data = lgb.Dataset(df[train_cols], df[target])
model      = lgb.train(params, train_data, valid_sets=train_data, num_boost_round=500)
