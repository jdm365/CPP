#include <iostream>
#include <vector>
#include <fstream>

#include <stdio.h>
#include <assert.h>
#include <bits/stdc++.h>

using namespace std;

class LinearRegression {
	vector<vector<float>> X;
	vector<float> y;

	int   dims;
	float beta_0;
	vector<float> beta_1;

	public:
		LinearRegression() {
			dims   = 0;
			beta_0 = 0;
		}

		void read_input(string filename) {
			ifstream file;
			file.open(filename);
			while (file.good()) {
				string line;
				getline(file, line, '\n');

				if (line == "") {
					break;
				}
				
				dims = 0;
				for (char c: line) {
					if (c == ',') {
						dims += 1;
					}
				}

				stringstream check1(line);
				string num;

				for (int idx = 0; idx < dims; idx++) {
					X.push_back(vector<float>());
					getline(check1, num, ',');
					float x_i = stof(num);
					X[idx].push_back(x_i);
				}

				getline(check1, num, ',');
				float y_i = stof(num);

				y.push_back(y_i);
			}
			file.close();

			assert (X[0].size() == y.size());
		}

		float get_x_sum(int N, int dim) {
			float x_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				x_sum += X[dim][idx];
			}
			return x_sum;
		}

		float get_y_sum(int N) {
			float y_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				y_sum += y[idx];
			}
			return y_sum;
		}

		float get_x_sqrd_sum(int N, int dim) {
			float x_sqrd_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				x_sqrd_sum += X[dim][idx] * X[dim][idx];
			}
			return x_sqrd_sum;
		}

		float get_y_sqrd_sum(int N) {
			float y_sqrd_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				y_sqrd_sum += y[idx] * y[idx];
			}
			return y_sqrd_sum;
		}

		float get_xy_sum(int N, int dim) {
			float xy_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				xy_sum += X[dim][idx] * y[idx];
			}
			return xy_sum;
		}

		void fit() {
			float N 		 = y.size();
			float y_sum 	 = get_y_sum(N);
			float y_sqrd_sum = get_y_sqrd_sum(N);

			for (int dim = 0; dim < dims; dim++) {
				float x_sum 	 = get_x_sum(N, dim);
				float x_sqrd_sum = get_x_sqrd_sum(N, dim);
				float xy_sum	 = get_xy_sum(N, dim);

				float beta_1_num    = N * xy_sum - x_sum * y_sum;
				float denom		    = (N - 1) * x_sqrd_sum;

				beta_1.push_back(beta_1_num / denom);
				
				if (dim == (dims - 1)) {
					float beta_0_num    = (y_sum * x_sqrd_sum) - (x_sum * xy_sum);
					beta_0 = beta_0_num / denom;
				}
			}

			cout << "The best fitting line is y = ";
			for (int idx = 0; idx < beta_1.size(); idx++) {
				cout << beta_1[idx] << " * x_" << idx << " + ";
			}
			cout << beta_0 << endl;
		}

		float predict(vector<float> X_pred) {
			float pred = 0;
			for (int idx = 0; idx < beta_1.size(); idx++) {
				pred += beta_1[idx] * X_pred[idx];
			}
			pred += beta_0;
			return pred;
		}

		float mse() {
			assert (X[0].size() == y.size());

			float error = 0;
			for (int idx = 0; idx < beta_1.size(); idx++) {
				float pred = predict(X[idx]);
				error += (pred - y[idx]) * (pred - y[idx]);
			}
			error = error / y.size();
			return error;
		}
};


int main() {
	LinearRegression regression;

	regression.read_input("data/iris_copy.csv");

	regression.fit();
	float error = regression.mse();
	cout << "Mean squared error: " << error << endl;

	return 0;
}
