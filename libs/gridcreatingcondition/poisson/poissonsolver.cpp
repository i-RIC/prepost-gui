#include "poissonsolver.h"

#include <algorithm>

namespace {

int index(int i, int j, int imax)
{
	return i + j * imax;
}

class Grid
{
public:
	Grid(std::vector<double>* x, std::vector<double>* y, int imax, int jmax) :
		m_x {x},
		m_y {y},
		m_iMax {imax},
		m_jMax {jmax}
	{}

	double& x(int i, int j)
	{
		return (*m_x)[index(i, j, m_iMax)];
	}

	double& y(int i, int j)
	{
		return (*m_y)[index(i, j, m_iMax)];
	}

	int iMax() const
	{
		return m_iMax;
	}
	int jMax() const
	{
		return m_jMax;
	}

private:

	std::vector<double>* m_x;
	std::vector<double>* m_y;

	int m_iMax;
	int m_jMax;
};

double updateGrid(Grid* grid)
{
//	double dxdxi, dxdet, dydxi, dydet;
//	double g00, g11, g12, g22;
//	double cof1, cof2, cof3, cof4, cof5;
//	double xold, yold, err, errx, erry;
	double err = 0;
	double errx = 0;
	double erry = 0;
	double sorcof = 1.2;
	for (int j = 1; j < grid->jMax() - 1; ++j) {
		for (int i = 1; i < grid->iMax() - 1; ++i) {
			double dxdxi = (grid->x(i + 1, j    ) - grid->x(i - 1,     j)) * 0.5;
			double dxdet = (grid->x(i    , j + 1) - grid->x(i    , j - 1)) * 0.5;
			double dydxi = (grid->y(i + 1, j    ) - grid->y(i - 1,     j)) * 0.5;
			double dydet = (grid->y(i    , j + 1) - grid->y(i    , j - 1)) * 0.5;
			double g11 = dxdxi * dxdxi + dydxi * dydxi;
			double g12 = dxdxi * dxdet + dydxi * dydet;
			double g22 = dxdet * dxdet + dydet * dydet;
//			double g00 = g11 * g22 - g12 * g12;
//			p[i, j] = -p[i, j] * g22 / g00;
//			q[i, j] = -q[i, j] * g11 / g00;
			double cof1 = g22;
			double cof2 = -g12 * 0.5;
			double cof3 = g11;
//			cof4 = g00 * p[i, j] * 0.5;
//			cof5 = g00 * q[i, j] * 0.5;
			double xold = grid->x(i, j);
			grid->x(i, j) =
					cof1 * (grid->x(i + 1, j    ) + grid->x(i - 1, j    ))
				+ cof3 * (grid->x(i    , j + 1) + grid->x(i    , j - 1))
//				+ cof4 * (x[i + 1, j] - x[i - 1, j])
//				+ cof5 * (x[i, j + 1] - x[i, j - 1])
				+ cof2 * (grid->x(i + 1, j + 1) + grid->x(i - 1, j - 1)
								- grid->x(i + 1, j - 1) - grid->x(i - 1, j + 1));
			grid->x(i, j) = (grid->x(i, j) * 0.5 / (cof1 + cof3) - xold) * sorcof + xold;
			double yold = grid->y(i, j);
			grid->y(i, j) =
					cof1 * (grid->y(i + 1, j    ) + grid->y(i - 1, j    ))
				+ cof3 * (grid->y(i    , j + 1) + grid->y(i    , j - 1))
//							+ cof4 * (y[i + 1, j] - y[i - 1, j])
//							+ cof5 * (y[i, j + 1] - y[i, j - 1])
				+ cof2 * (grid->y(i + 1, j + 1) + grid->y(i - 1, j - 1)
								- grid->y(i + 1, j - 1) - grid->y(i - 1, j + 1));
			grid->y(i, j) = (grid->y(i, j) * 0.5 / (cof1 + cof3) - yold) * sorcof + yold;

			if (grid->x(i, j) != 0) {
				err = abs(grid->x(i, j) - xold) / grid->x(i, j);
			}
			errx = std::max(errx, err);
			if (grid->y(i, j) != 0) {
				err = abs(grid->y(i, j) - yold) / grid->y(i, j);
			}
			erry = std::max(erry, err);
		}
	}
	err = std::max(errx, erry);
	return err;
}

} // namespace

void PoissonSolver::solve(std::vector<double>* x, std::vector<double>* y, int imax, int jmax, double eps, int maxIter)
{
//	return;

	int count = 0;
	double err = 0;

	std::vector<double> originalX = *x;
	std::vector<double> originalY = *y;

	Grid grid(x, y, imax, jmax);

	do {
		++ count;

		err = updateGrid(&grid);

		if (count > maxIter) {break;}
	} while (err > eps);
}

PoissonSolver::PoissonSolver()
{}

