#ifndef ANMS_H
#define ANMS_H

#include "misc_global.h"

#include <vector>

typedef double optimfn(int n, const double *x0, void *ex);

// Adaptive Nelder - Mead Simplex(ANMS) algorithm
class MISCDLL_EXPORT ANMS
{
private:
	class vertex {
	public:
		std::vector<double> x;
		double v;
	public:
		vertex(const std::vector<double>&x, double v);
		static bool compare(const vertex *p1, const vertex *p2);
	};

public:
	ANMS() = default;
	ANMS(const ANMS&) = delete;
	ANMS& operator=(const ANMS&) = delete;
	~ANMS();

	void minimize(optimfn fminfn, void *ex, const std::vector<double>& x0, int maxit, double ftol, double xtol);
	double minimizeStep(optimfn fminfn, const std::vector<double>& x, void* ex);

	int iterations = 0;
	int fncount = 0;
	std::vector<double> xout;
	double fmin = 0;

private:
	std::vector<vertex*> simplex;
	void resize(int m);
	std::vector<double> centroid(int h);
};

#endif // ANMS_H
