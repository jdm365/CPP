from GBDT import GBDT
import pandas as pd
import os


if __name__ == '__main__':
    model = GBDT(
            num_boosting_rounds=1000,
            max_leaves=31, 
            max_depth=5,
            min_data_in_leaf=20, 
            col_subsample_rate=0.6,
            max_bin=255,
            lr=0.1,
            dart=True,
            verbosity=50,
            early_stopping_steps=50,
            gpu=False
            )

    #FILEPATH = '../data/iris_dataset.csv'
    FILEPATH = '../data/housing_price_prediction_dataset.csv'
    #FILEPATH = '../data/hpx10.csv'
    #FILEPATH = '../data/hpx100.csv'
    #FILEPATH = '../data/preprocessed_train.feather'

    FILEPATH = os.path.join(os.path.dirname(__file__), FILEPATH)
    df = pd.read_csv(FILEPATH)

    model.train_eval(df)


