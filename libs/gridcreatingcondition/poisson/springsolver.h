#ifndef SPRINGSOLVER_H
#define SPRINGSOLVER_H

#include <vector>

class SpringSolver
{
public:
	static void solve(std::vector<double>* x, std::vector<double>* y, int imax, int jmax, double eps, int maxIter, int* progress, bool* cancel);

private:
	SpringSolver();
};

#endif // SPRINGSOLVER_H
