#include <stdio.h>
#include <stdlib.h>



#define N 1000000000

void vector_add(float* out, float* a, float* b, int n) {
	for (int idx = 0; idx < n; ++idx) {
		out[idx] = a[idx] + b[idx];
	}
}

int main() {
	float* a   = (float*)malloc(sizeof(float) * N);
	float* b   = (float*)malloc(sizeof(float) * N);
	float* out = (float*)malloc(sizeof(float) * N);

	for (int idx = 0; idx < N; ++idx) {
		a[idx] = 1.00f;
		b[idx] = 2.00f;
	}

    vector_add(out, a, b, N);

	free(a);
	free(b);
	free(out);
	return 0;
}
