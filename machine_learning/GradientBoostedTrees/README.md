Gradient Boosted Tree Library

Python accessible library for gradient boosted tree regressors and classifiers with backend in C/C++.

Currently only Histogram Gradient Boosted tree methods are supproted throught the python api.

This project is entirely for fun and though it works and is roughly the same speed as LightGBM/Sklearn,
it is not as featurefull and is far less thoroughly tested and maintained.



To compile:
```
cd build
cmake ../ ; make all
```

To access python module:
```
cd python
python
> import GBDT
```
