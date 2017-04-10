#ifndef POISSONSOLVER_H
#define POISSONSOLVER_H

#include <vector>

class PoissonSolver
{
public:
	static void solve(std::vector<double>* x, std::vector<double>* y, int imax, int jmax, double eps, int maxIter);

private:
	PoissonSolver();
};

#endif // POISSONSOLVER_H
