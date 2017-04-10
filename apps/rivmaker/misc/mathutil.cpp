#include "mathutil.h"

void MathUtil::leastSquares(const std::vector<double>& x, std::vector<double>& y, double* a, double* b)
{
	double a00 = 0, a01 = 0, a02 = 0, a11 = 0, a12 = 0;

	for (int i = 0; i < x.size(); ++i) {
		a00 += 1;
		a01 += x[i];
		a02 += y[i];
		a11 += x[i] * x[i];
		a12 += x[i] * y[i];
	}
	double M = a00 * a11 - a01 * a01;

	*b = (a02 * a11 - a01 * a12) / M;
	*a = (a00 * a12 - a01 * a02) / M;
}

MathUtil::MathUtil()
{}
