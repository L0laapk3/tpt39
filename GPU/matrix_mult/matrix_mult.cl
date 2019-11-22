#define M 500
#define K M
#define N M


__kernel void matrix_mult(__global const float *x, __global const float *y, __global float *restrict z)
{

	unsigned m = get_global_id(0);
	unsigned n = get_global_id(1);
	z[m + n * M] = 0;
	for (unsigned k = 0; k < K; k++)
		// z[m + n * M] += x[k + n * M] * y[m * N + k];			//transposed version
		z[m + n * M] += x[k + n * M] * y[m + k * M];			//normal version
}

