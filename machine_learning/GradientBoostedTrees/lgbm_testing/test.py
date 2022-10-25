import pandas as pd
import lightgbm as lgb


## df = pd.read_csv('../data/housing_price_prediction_dataset.csv', header=None)
df = pd.read_csv('../data/hpx10.csv', header=None)

params = {
        'boosting': 'gbdt',
        'num_threads': 0,
        'learning_rate': 0.1,
        'max_depth': 8,
        'max_leaves': 1000,
        'min_child_weight': 0.00,
        'objective': 'regression',
        'verbosity': '-1'
        }
ds_params = {
        'is_enable_sparse': 'true',
        'enable_bundle': 'true',
        'max_bin': 255
        }

train_cols = df.columns[:-1]
target     = df.columns[-1]
train_data = lgb.Dataset(df[train_cols], df[target], params=ds_params)
model      = lgb.train(params, train_data, valid_sets=train_data, num_boost_round=50)
