import GBDTEngine
import numpy as np
import pandas as pd
import typing


arrayable = typing.Union[np.ndarray, typing.List]




class GBDT:
    def __init__(
            self,
            max_depth:           int = -1,
            l2_reg:              float = 0.00,
            lr:                  float = 0.50,
            min_child_weight:    float = 1.0,
            min_data_in_leaf:    int = 20,
            num_boosting_rounds: int = 100,
            max_bin:             int = 255,
            max_leaves:          int = 127,
            col_subsample_rate:  float = 0.8,
            dart:                bool = False
            ):

        self.engine = GBDTEngine.GBM(
                max_depth,
                l2_reg,
                lr,
                min_child_weight,
                min_data_in_leaf,
                num_boosting_rounds,
                max_bin,
                max_leaves,
                col_subsample_rate,
                dart
                )



    def train_hist(self, X: arrayable, y: arrayable) -> None:
        X = self.convert_to_np_float32(X)
        y = self.convert_to_np_float32(y)

        self.engine.train_hist(X=X, y=y)

    
    def convert_to_np_float32(self, X: arrayable) -> np.ndarray:
        return np.array(X, dtype=np.float32)







if __name__ == '__main__':
    model = GBDT()

    df = pd.read_csv('data/hpx10.csv')
    X  = df[df.columns[:-1]]
    y  = df[df.columns[-1]]

    model.train_hist(X, y)
