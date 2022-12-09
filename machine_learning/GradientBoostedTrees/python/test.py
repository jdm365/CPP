import numpy as np
import pandas as pd
from GBDT import train_hist

df = pd.read_csv('hpx10.csv')
X = np.array(df[df.columns[:-1]], dtype=np.float32)
y = np.array(df[df.columns[-1]], dtype=np.float32)

train_hist(X, y)
