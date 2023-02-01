import GBDTEngine
import numpy as np
import pandas as pd
from typing import Union, List
import random
import joblib


arrayable   = Union[np.ndarray, List, pd.DataFrame]

class GBDT:
    def __init__(
            self,
            max_depth:            int = -1,
            l2_reg:               float = 0.00,
            lr:                   float = 0.50,
            min_child_weight:     float = 1.0,
            min_data_in_leaf:     int = 20,
            num_boosting_rounds:  int = 500,
            max_bin:              int = 255,
            max_leaves:           int = 31,
            col_subsample_rate:   float = 0.8,
            dart:                 bool = False,
            verbosity:            int = 1,
            early_stopping_steps: int = 10,
            gpu:                  bool = False
            ):
        assert max_bin < 256, 'max_bin must be <= 255'

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
        self.early_stopping_steps = early_stopping_steps
        self.gpu = gpu


    def train_hist(
            self, 
            X: arrayable, 
            y: arrayable, 
            X_validation: arrayable = None, 
            y_validation: arrayable = None
            ) -> None:
        X = self.convert_to_np_float32(X)
        y = self.convert_to_np_float32(y)

        if (X_validation is not None) and (y_validation is not None):
            X_validation = self.convert_to_np_float32(X_validation)
            y_validation = self.convert_to_np_float32(y_validation)

        self.engine.train_hist(
                X=X, 
                y=y,
                X_validation=X_validation,
                y_validation=y_validation,
                early_stopping_steps=self.early_stopping_steps
                )

    def train_hist_gpu(
            self, 
            X: arrayable, 
            y: arrayable, 
            X_validation: arrayable = None, 
            y_validation: arrayable = None
            ) -> None:
        X = self.convert_to_np_float32(X)
        y = self.convert_to_np_float32(y)

        if (X_validation is not None) and (y_validation is not None):
            X_validation = self.convert_to_np_float32(X_validation)
            y_validation = self.convert_to_np_float32(y_validation)

        self.engine.train_hist_gpu(
                X=X, 
                y=y
                )


    def predict_hist(self, X: arrayable) -> np.ndarray:
        X = self.convert_to_np_float32(X)
        return self.engine.predict_hist(X=X)
    

    def convert_to_np_float32(self, X: arrayable) -> np.ndarray:
        return np.array(X, dtype=np.float32)


    def train_eval(
            self, 
            train_data: arrayable, 
            X_eval: np.ndarray = None, 
            y_eval: np.ndarray = None, 
            train_size: float = 0.75, 
            random_seed: int = 42
            ) -> None:
        ## Assumes target col is last col.

        df_train, df_test = self.train_test_split(train_data)

        X_train = df_train[df_train.columns[:-1]]
        y_train = df_train[df_train.columns[-1]]

        X_test = df_test[df_test.columns[:-1]]
        y_test = self.convert_to_np_float32(df_test[df_test.columns[-1]])

        if self.gpu:
            self.train_hist_gpu(X_train, y_train)
        else:
            self.train_hist(X_train, y_train, X_test, y_test)

        if X_eval is not None:
            preds = model.predict_hist(X_eval)
            print(f'Test MSE: {model._calc_mse(preds, y_eval)}')


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
            data = self._integer_encode(data)
            data = data.fillna(-1)
            train_data = data.iloc[train_idxs]
            test_data  = data.iloc[test_idxs]
        else:
            train_data = data[train_idxs]
            test_data  = data[test_idxs]

        return train_data, test_data


    def _integer_encode(self, df: pd.DataFrame) -> None:
        for idx, dtype in enumerate(df.dtypes):
            if dtype not in [str, object]:
                continue
            col = df.columns[idx]
            mapping = {value: key for key, value in dict(enumerate(pd.unique(df[col]))).items()}
            df[col] = df[col].map(mapping)
        return df


    def _calc_mse(self, preds: np.ndarray, true: np.ndarray) -> float:
        return 0.5 * np.sum((preds - true) * (preds - true)) / len(preds)




if __name__ == '__main__':
    model = GBDT(
            num_boosting_rounds=1000,
            max_leaves=31, 
            max_depth=-1,
            min_data_in_leaf=20, 
            col_subsample_rate=1.0,
            max_bin=255,
            lr=0.5,
            dart=False,
            verbosity=50,
            early_stopping_steps=-1,
            gpu=True
            )

    EPSILON = False

    #df = pd.read_csv('../data/iris_dataset.csv')
    df = pd.read_csv('../data/housing_price_prediction_dataset.csv')
    #df = pd.read_csv('../data/hpx10.csv')
    #df = pd.read_csv('../data/hpx100.csv')
    #df = pd.read_feather('../data/preprocessed_train.feather')

    if EPSILON:
        from catboost.dataset import epsilon
        df_train, df_test = epsilon()
        model.train_eval(df_train)
    else:
        model.train_eval(df)


