#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <vector>

class MathUtil
{
public:
	static void leastSquares(const std::vector<double>& x, std::vector<double>& y, double* a, double* b);

private:
	MathUtil();
};

#endif // MATHUTIL_H
