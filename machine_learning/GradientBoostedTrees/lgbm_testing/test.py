import pandas as pd
import lightgbm as lgb


df = pd.read_csv('../data/housing_price_prediction_dataset.csv', header=None)
params = {
        ##'num_threads': 1,
        'learning_rate': 0.5,
        'max_depth': 8,
        'max_leaves': 1000,
        'verbosity': '-1'
        }
ds_params = {
        'is_enable_sparse': 'true',
        'enable_bundle': 'true'
        }

train_cols = df.columns[:-1]
target     = df.columns[-1]
train_data = lgb.Dataset(df[train_cols], df[target], params=ds_params)
model      = lgb.train(params, train_data, valid_sets=train_data, num_boost_round=250)
