#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

class LinearRegression {
	vector<float> x;
	vector<float> y;

	float m;
	float b;

	float sum_xy;
	float sum_x;
	float sum_y;
	float sum_x_square;
	float sum_y_square;

public:
	regression() {
		m=0;
		b=0;
		sum_xy=0;
		sum_x=0;
		sum_y=0;
		sum_x_square=0;
		sum_y_square=0;
	}

	void calculate_coefficient() {
		float N = x.size();
		float numerator   = N * sum_xy - sum_x * sum_y;
		float denominator = N * sum_x_square - sum_x * sum_x;
		m = numerator / denominator;
	}


	void calculate_intercept() {
		float N = x.size();
		float numerator   = sum_y * sum_x_square - sum_x * sum_xy;
		float denominator = N * sum_x_square - sum_x * sum_x;
		b = numerator / denominator;
	}

	int n_rows() {
		return x.size();
	}

	float coefficient() {
		if (b == 0)
			calculate_coefficient();
		return b;
	}
	
	void print_line() {
		if (m == 0 && b == 0) {
			calculate_coefficient();
			calculate_intercept();
		}
		cout << "The best fitting line is y = ";
		cout << m << "x + " << b << endl;
	}
	
	void get_input(int n) {
		for (int i = 0; i < n; i++) {
			char comma;
			float x_i;
			float y_i;
			cin >> x_i >> comma >> y_i;
			sum_xy += x_i * y_i;
			sum_x += x_i;
			sum_y += y_i;
			sum_x_square += x_i * x_i;
			sum_y_square += y_i * y_i;
			x.push_back(x_i);
			y.push_back(y_i);
		}
	}

	void show_data {
		for (int i = 0; i< 62; i++) {
			printf("_");
		}

		printf("\n\n");
		printf("|%15s%5s %15s%5s%20s\n", "X", "", "Y", "", "|");

		for (int = 0; i < x.size(); i++) {
			printf("|%20f %20f%20s\n", x[i], y[i], "|");
		}

		for (int i = 0; i< 62; i++) {
			printf("_");
		}
		printf("\n\n");
	}

	float predict(float x) {
		return m * x + b;
	}

	float mse() {
		float error = 0;
		for (int i = 0; i < x.size(); i++) {
			error += ((predict(x[i]) - y[i]) * (predict(x[i]) - y[i]));
		}
		return error;
	}

	float get_residuals(float num) {
		for (int i = 0; i < x.size(); i++) {
			if (num == x[i]) {
				return (y[i] - predict(x[i]));
			}
		}
		return 0;
	}
};

int main() {
	freopen("input.txt", "r", stdin);
	regression reg;

	int n;
	cin >> n;

	reg.get_input(n);

	reg.print_line();
	cout << "Predicted value at 2060 = " << reg.predict(2060) << endl;
	cout << "MSE: " << reg.mse() << endl;
	cout << "Error in 2050 = " << reg.get_residuals(2050) << endl;
}
		



