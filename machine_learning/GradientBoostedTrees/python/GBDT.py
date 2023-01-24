import GBDTEngine
import numpy as np
import pandas as pd
from typing import Union, List
import random
import joblib


arrayable = Union[np.ndarray, List, pd.DataFrame]




class GBDT:
    def __init__(
            self,
            max_depth:           int = -1,
            l2_reg:              float = 0.00,
            lr:                  float = 0.50,
            min_child_weight:    float = 1.0,
            min_data_in_leaf:    int = 20,
            num_boosting_rounds: int = 50,
            max_bin:             int = 255,
            max_leaves:          int = 31,
            col_subsample_rate:  float = 0.8,
            dart:                bool = False,
            verbosity:           int = 1
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
                dart,
                verbosity
                )


    def train_hist(self, X: arrayable, y: arrayable) -> None:
        X = self.convert_to_np_float32(X)
        y = self.convert_to_np_float32(y)

        self.engine.train_hist(X=X, y=y)


    def predict_hist(self, X: arrayable) -> np.ndarray:
        X = self.convert_to_np_float32(X)
        return self.engine.predict_hist(X=X)
    

    def convert_to_np_float32(self, X: arrayable) -> np.ndarray:
        return np.array(X, dtype=np.float32)


    def train_eval(
            self, 
            data: arrayable, 
            train_size: float = 0.75, 
            random_seed: int = 42
            ) -> None:
        df_train, df_test = self.train_test_split(df)

        X_train = df_train[df_train.columns[:-1]]
        y_train = df_train[df_train.columns[-1]]

        X_test = df_test[df_test.columns[:-1]]
        y_test = self.convert_to_np_float32(df_test[df_test.columns[-1]])

        self.train_hist(X_train, y_train)
        preds = model.predict_hist(X_test)
        print(f'Test MSE: {model.calc_mse(preds, y_test)}')


    def train_test_split(
            self, 
            data: arrayable, 
            train_size: float = 0.75,
            random_seed: int = 42
            ) -> (np.ndarray, np.ndarray):
        random.seed(random_seed)

        data_size = len(data)
        idxs = np.arange(data_size)

        np.random.shuffle(idxs)

        train_idxs = idxs[:int(data_size * train_size)]
        test_idxs  = idxs[int(data_size * train_size):]

        if type(data) == pd.DataFrame:
            train_data = data.iloc[train_idxs]
            test_data  = data.iloc[test_idxs]
        else:
            train_data = data[train_idxs]
            test_data  = data[test_idxs]

        return train_data, test_data



    def calc_mse(self, preds: np.ndarray, true: np.ndarray) -> float:
        return np.sum((preds - true) * (preds - true)) / len(preds)




if __name__ == '__main__':
    model = GBDT(num_boosting_rounds=25, lr=0.1, dart=True)

    df = pd.read_csv('../data/housing_price_prediction_dataset.csv')
    #df = pd.read_csv('../data/hpx100.csv')
    model.train_eval(df)
