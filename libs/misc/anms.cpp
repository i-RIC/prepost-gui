#include "anms.h"
#include "dvector.h"

#include <algorithm>

ANMS::vertex::vertex(const std::vector<double>&x, double v) :
	x(x), v(v)

{}

bool ANMS::vertex::compare(const vertex *p1, const vertex *p2)
{
	return (p1->v < p2->v);
}

void ANMS::resize(int m) {
	if (! simplex.empty()) {
		for (auto e : simplex) delete e;
		simplex.resize(0);
	}
	if (m > 0) simplex.resize(m, nullptr);
}

std::vector<double> ANMS::centroid(int h)
{
	int n = simplex.size() - 1;
	std::vector<double> c(n, 0.0);
	int m = 0;
	for (int i = 0; i < n + 1; i++) {
		if (i == h) continue;
		c += simplex[i]->x;
		m++;
	}
	return c / m;
}

ANMS::~ANMS() {
	resize(0);
}

void ANMS::minimize(optimfn fminfn, void *ex, const std::vector<double>& x0, int maxit, double ftol, double xtol)
{
	// parameters of transformations
	int n = x0.size();
	double alpha = 1.0;
	double beta = 1.0 + 2.0 / n;
	double gamma = 0.75 - 1.0 / 2.0 / n;
	double delta = 1.0 - 1.0 / n;

	// number of function calls
	fncount = 0;

	// initialize a simplex and function values
	resize(n + 1);
	double fvalue = minimizeStep(fminfn, x0, ex);
	for (auto& e : simplex) {
		e = new vertex(x0, fvalue);
	}
	for (int i = 1; i < n + 1; i++) {
		double tau = (x0[i - 1] == 0.0 ? 0.00025 : 0.05 * x0[i - 1]);
		simplex[i]->x[i - 1] += tau;
		simplex[i]->v = minimizeStep(fminfn, simplex[i]->x, ex);
	}
	std::sort(simplex.begin(), simplex.end(), vertex::compare);

	// centroid cache
	std::vector<double> c = centroid(n);

	// lowest index
	std::vector<double> xl = simplex[0]->x;
	double fl = simplex[0]->v;

	// stopping criteria
	iterations = 0;  // number of iterations
	bool domconv = false;  // domain convergence
	bool fvalconv = false;  // function-value convergence

	while ((iterations < maxit) && !(fvalconv && domconv)) {
		// highest and second highest indices
		std::vector<double> xh = simplex[n]->x;
		double fh = simplex[n]->v;
		double fs = simplex[n - 1]->v;

		// reflect
		std::vector<double> xr = c + (c - xh) * alpha;
		double fr = minimizeStep(fminfn, xr, ex);
		bool doshrink = false;
		std::vector<double> x;
		double fvalue = 0;

		if (fr < fl) { // <= fs
			// expand
			std::vector<double> xe = c + (xr - c) * beta;
			double fe = minimizeStep(fminfn, xe, ex);
			if (fe < fr) {
					x = xe;
					fvalue = fe;
			} else {
				x = xr;
				fvalue = fr;
			}
		} else if (fr < fs) {
			x = xr;
			fvalue = fr;
		} else { // fs <= fr
			// contract
			if (fr < fh) {
				// outside
				std::vector<double> xc = c + (xr - c) * gamma;
				double fc = minimizeStep(fminfn, xc, ex);
				if (fc <= fr) {
					x = xc;
					fvalue = fc;
				} else {
					doshrink = true;
				}
			} else {
				// inside
				std::vector<double> xc = c - (xr - c) * gamma;
				double fc = minimizeStep(fminfn, xc, ex);
				if (fc < fh) {
					x = xc;
					fvalue = fc;
				} else {
					doshrink = true;
				}
			}
		}
		// update simplex, function values and centroid cache
		// shrinkage almost never happen in practice
		if (doshrink) {
			// shrink
			for (int i = 1; i < n + 1; i++) {
				simplex[i]->x = xl + (simplex[i]->x - xl) * delta;
				simplex[i]->v = minimizeStep(fminfn, simplex[i]->x, ex);
			}
			std::sort(simplex.begin(), simplex.end(), vertex::compare);
			c = centroid(n);
		} else {
			// insert the new function value into an ordered position
			simplex[n]->x = x;
			simplex[n]->v = fvalue;
			for (int i = n; i > 0; i--) {
				if (simplex[i - 1]->v > simplex[i]->v) {
					vertex *tmp = simplex[i - 1];
					simplex[i - 1] = simplex[i];
					simplex[i] = tmp;
				} else {
					break;
				}
			}
			// add the new vertex, and subtract the highest vertex
			xh = simplex[n]->x;
			c += (x - xh) / n;
		}

		xl = simplex[0]->x;
		fl = simplex[0]->v;

		// check convergence
		fvalconv = true;
		for (int i = 1; i < n + 1; i++) {
			if (std::abs(simplex[i]->v - fl) > ftol) {
				fvalconv = false;
				break;
			}
		}
		domconv = true;
		for (int i = 1; i < n + 1; i++) {
			for (int j = 0; j < n; j++) {
				if (std::abs(simplex[i]->x[j] - xl[j]) > xtol) {
					domconv = false;
					break;
				}
			}
			if (!domconv) break;
		}

		iterations++;
	}

	// return the lowest vertex (or the centroid of the simplex) and the function value
	c = centroid(-1);
	double fcent = minimizeStep(fminfn, c, ex);
	if (fcent < fl) {
		xout = c;
		fmin = fcent;
	} else {
		xout = xl;
		fmin = fl;
	}
}

double ANMS::minimizeStep(optimfn fminfn, const std::vector<double>& x, void* ex)
{
	int size = static_cast<int>(x.size());
	const double* data = x.data();

	fncount++;
	return (*fminfn)(size, data, ex);
}
