#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <assert.h>

using namespace std;

class LinearRegression {
	vector<float> x;
	vector<float> y;

	float beta_0;
	float beta_1;

	public:
		LinearRegression() {
			beta_0 = 0;
			beta_1 = 0;
		}

		void get_input(int n) {
			for (int i = 0; i < n; i++) {
				char comma;
				float x_i;
				float y_i;
				cin >> x_i >> comma >> y_i;
				x.push_back(x_i);
				y.push_back(y_i);
			}
			assert (x.size() == y.size();
		}

		float get_x_sum(int N) {
			float x_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				float x_sum += x[idx];
			}
			return x_sum;
		}

		float get_y_sum(int N) {
			float y_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				float y_sum += y[idx];
			}
			return y_sum;
		}

		float get_x_sqrd_sum(int N) {
			float x_sqrd_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				float x_sqrd_sum += x[idx] * x[idx];
			}
			return x_sqrd_sum;
		}

		float get_y_sqrd_sum(int N) {
			float y_sqrd_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				float y_sqrd_sum += y[idx] * y[idx];
			}
			return y_sqrd_sum;
		}

		float get_xy_sum(int N) {
			float xy_sum = 0;
			for (int idx = 0; idx < N; idx++) {
				float xy_sum += x[idx] * y[idx];
			}
			return xy_sum;
		}

		void get_model() {
			float N 		 = x.size();
			float x_sum 	 = get_x_sum(N);
			float y_sum 	 = get_y_sum(N);
			float x_sqrd_sum = get_x_sqrd_sum(N);
			float y_sqrd_sum = get_y_sqrd_sum(N);
			float xy_sum	 = get_xy_sum(N);

			float beta_0_num    = (y_sum * x_sqrd_sum) - (x_sum * xy_sum)
			float beta_1_num    = N * sum_xy - sum_x * sum_y;
			float denom		    = (N - 1) * x_sqrd_sum;

			beta_0 = beta_0_num / denom;
			beta_1 = beta_1_num / denom;

			cout << "The best fitting line is y = ";
			cout << beta_1 << "x + " << beta_0 << endl;
		}

		float predict(float x) {
			return beta_1 * x + beta_0;
		}

		float mse(vector<float> predictions) {
			float error = 0;
			for (int idx = 0; idx < x.size(); idx++) {
				error += (predictions - y[i]) * (predictions - y[i]));
			}
			return error;
		}
};


int main() {
	freopen("input.csv", "r", stdin);
	LinearRegression reg;

	int n;
	cin >> n;

	reg.get_input(n);

	reg.print_line();
	cout << "Predicted value at 2060 = " << reg.predict(2060) << endl;
	cout << "MSE: " << reg.mse() << endl;
	cout << "Error in 2050 = " << reg.get_residuals(2050) << endl;

	return 0;
}
