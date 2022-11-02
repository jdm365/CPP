import time
import pandas as pd
import numpy  as np 
from sklearn.ensemble import HistGradientBoostingRegressor

## df = pd.read_csv('../../data/housing_price_prediction_dataset.csv', header=None)
df = pd.read_csv('../../data/hpx10.csv', header=None)

train_cols = df.columns[:-1]
target     = df.columns[-1]

model = HistGradientBoostingRegressor(
        max_leaf_nodes=1000, 
        max_depth=8,
        early_stopping=False,
        verbose=1,
        categorical_features=None
        )

model.fit(np.array(df[train_cols]), np.array(df[target]))
