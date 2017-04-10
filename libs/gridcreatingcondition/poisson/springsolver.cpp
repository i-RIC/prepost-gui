#include "springsolver/edge.h"
#include "springsolver/grid.h"
#include "springsolver.h"

#include <QVector2D>

#include <algorithm>

namespace {

const double ROOT2 = 1.41421356;

void calcDelta(Grid* grid, int i1, int j1, int i2, int j2, double* dx, double* dy)
{
	double x1 = grid->x(i1, j1);
	double x2 = grid->x(i2, j2);
	double y1 = grid->y(i1, j1);
	double y2 = grid->y(i2, j2);

	*dx = x2 - x1;
	*dy = y2 - y1;
}

double edgeLen(Grid* grid, int i1, int j1, int i2, int j2)
{
	double dx, dy;
	calcDelta(grid, i1, j1, i2, j2, &dx, &dy);

	return sqrt(dx * dx + dy * dy);
}

double averageEdgeLength(Grid* grid) {
	int count = 0;
	double sum = 0;

	for (int i = 0; i < grid->iMax() - 1; ++i) {
		for (int j = 0; j < grid->jMax() - 1; ++j) {
			sum += edgeLen(grid, i    , j, i + 1, j    );
			sum += edgeLen(grid, i    , j, i    , j + 1);
			sum += edgeLen(grid, i    , j, i + 1, j + 1) / ROOT2;
			sum += edgeLen(grid, i + 1, j, i    , j + 1) / ROOT2;
			count += 4;
		}
	}
	return sum / count;
}

void applyLengthLimit(double *vx, double* vy, double maxLen)
{
	QVector2D v(*vx, *vy);
	if (v.lengthSquared() < maxLen * maxLen) {return;}

	QVector2D newV = v.normalized() * maxLen;
	*vx = newV.x();
	*vy = newV.y();
}

void calcForce(const std::vector<double>& dxVec, const std::vector<double>& dyVec, const std::vector<double>& cVec, double springK, double* forceX, double* forceY, double* avrLen)
{
	std::vector<double> lenVec;
	double sum = 0;

	for (int i = 0; i < dxVec.size(); ++i) {
		double dx = dxVec.at(i);
		double dy = dyVec.at(i);
		double c = cVec.at(i);

		double len = sqrt(dx * dx + dy * dy) * c;
		lenVec.push_back(len);
		sum += len;
	}
	*avrLen = sum / dxVec.size();
	double deltaLimit = *avrLen * 0.15;

	*forceX = 0;
	*forceY = 0;

	for (int i = 0; i < dxVec.size(); ++i) {
//		double deltaLen = sqrt(*avrLen) * sqrt(lenVec.at(i)) * springK;
		double deltaLen = lenVec.at(i) * springK;
		QVector2D v(dxVec.at(i), dyVec.at(i));
		v = v.normalized() * deltaLen;
		*forceX += v.x();
		*forceY += v.y();
	}
	applyLengthLimit(forceX, forceY, deltaLimit);
}

double updateGrid(Grid* grid, double springK)
{
	std::vector<double> origX, origY;

	origX = *(grid->xVec());
	origY = *(grid->yVec());

	Grid origGrid(&origX, &origY, grid->iMax(), grid->jMax());

	int springCount = 8;

	double err = 0;

	springK /= springCount;

	// inner region
	for (int j = 1; j < grid->jMax() - 1; ++j) {
		for (int i = 1; i < grid->iMax() - 1; ++i) {
			std::vector<double> dxVec, dyVec, cVec;
			double dx, dy;

			calcDelta(&origGrid, i, j, i - 1, j    , &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

			calcDelta(&origGrid, i, j, i + 1, j    , &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

			calcDelta(&origGrid, i, j, i    , j - 1, &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

			calcDelta(&origGrid, i, j, i    , j + 1, &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

			calcDelta(&origGrid, i, j, i - 1, j - 1, &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

			calcDelta(&origGrid, i, j, i - 1, j + 1, &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

			calcDelta(&origGrid, i, j, i + 1, j - 1, &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

			calcDelta(&origGrid, i, j, i + 1, j + 1, &dx, &dy);
			dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

			double forceX, forceY, avrLen;
			calcForce(dxVec, dyVec, cVec, springK, &forceX, &forceY, &avrLen);

			grid->x(i, j) = origGrid.x(i, j) + forceX;
			grid->y(i, j) = origGrid.y(i, j) + forceY;

			err = std::max(err, std::abs(forceX) / avrLen);
			err = std::max(err, std::abs(forceY) / avrLen);
		}
	}

	// i = 0
	auto& iMinEdge = grid->iMinEdge();
	auto& iMinEdgeIds = grid->iMinEdgeIds();
	for (int j = 1; j < grid->jMax() - 1; ++j) {
		int edgeId = iMinEdgeIds.at(j - 1);
		QVector2D edgeDir = iMinEdge.lineDirection(edgeId);

		std::vector<double> dxVec, dyVec, cVec;
		double dx, dy;

		calcDelta(&origGrid, 0, j, 1, j    , &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, 0, j, 0    , j - 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, 0, j, 0    , j + 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, 0, j, 1, j - 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		calcDelta(&origGrid, 0, j, 1, j + 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		double forceX, forceY, avrLen;
		calcForce(dxVec, dyVec, cVec, springK, &forceX, &forceY, &avrLen);

		double r = forceX * edgeDir.x() + forceY * edgeDir.y();
		forceX = r * edgeDir.x();
		forceY = r * edgeDir.y();

		double tmpNewX = origGrid.x(0, j) + forceX;
		double tmpNewY = origGrid.y(0, j) + forceY;

		int newEdgeId;
		double newX, newY;
		iMinEdge.findMappedPoint(tmpNewX, tmpNewY, edgeId, &newX, &newY, &newEdgeId);
		iMinEdgeIds[j - 1] = newEdgeId;

		grid->x(0, j) = newX;
		grid->y(0, j) = newY;

		err = std::max(err, std::abs(newX - origGrid.x(0, j)) / avrLen);
		err = std::max(err, std::abs(newY - origGrid.y(0, j)) / avrLen);
	}

	// i = imax
	auto& iMaxEdge = grid->iMaxEdge();
	auto& iMaxEdgeIds = grid->iMaxEdgeIds();
	int imax_1 = grid->iMax() - 1;
	for (int j = 1; j < grid->jMax() - 1; ++j) {
		int edgeId = iMaxEdgeIds.at(j - 1);
		QVector2D edgeDir = iMaxEdge.lineDirection(edgeId);

		std::vector<double> dxVec, dyVec, cVec;
		double dx, dy;

		calcDelta(&origGrid, imax_1, j, imax_1 - 1, j    , &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, imax_1, j, imax_1    , j - 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, imax_1, j, imax_1    , j + 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, imax_1, j, imax_1 - 1, j - 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		calcDelta(&origGrid, imax_1, j, imax_1 - 1, j + 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		double forceX, forceY, avrLen;
		calcForce(dxVec, dyVec, cVec, springK, &forceX, &forceY, &avrLen);

		double r = forceX * edgeDir.x() + forceY * edgeDir.y();
		forceX = r * edgeDir.x();
		forceY = r * edgeDir.y();

		double tmpNewX = origGrid.x(imax_1, j) + forceX;
		double tmpNewY = origGrid.y(imax_1, j) + forceY;

		int newEdgeId;
		double newX, newY;
		iMaxEdge.findMappedPoint(tmpNewX, tmpNewY, edgeId, &newX, &newY, &newEdgeId);
		iMaxEdgeIds[j - 1] = newEdgeId;

		grid->x(imax_1, j) = newX;
		grid->y(imax_1, j) = newY;

		err = std::max(err, std::abs(newX - origGrid.x(imax_1, j)) / avrLen);
		err = std::max(err, std::abs(newY - origGrid.y(imax_1, j)) / avrLen);
	}

	// j = 0
	auto& jMinEdge = grid->jMinEdge();
	auto& jMinEdgeIds = grid->jMinEdgeIds();
	for (int i = 1; i < grid->iMax() - 1; ++i) {
		int edgeId = jMinEdgeIds.at(i - 1);
		QVector2D edgeDir = jMinEdge.lineDirection(edgeId);

		std::vector<double> dxVec, dyVec, cVec;
		double dx, dy;

		calcDelta(&origGrid, i, 0, i    , 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, i, 0, i - 1, 0, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, i, 0, i + 1, 0, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, i, 0, i - 1, 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		calcDelta(&origGrid, i, 0, i + 1, 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		double forceX, forceY, avrLen;
		calcForce(dxVec, dyVec, cVec, springK, &forceX, &forceY, &avrLen);

		double r = forceX * edgeDir.x() + forceY * edgeDir.y();
		forceX = r * edgeDir.x();
		forceY = r * edgeDir.y();

		double tmpNewX = origGrid.x(i, 0) + forceX;
		double tmpNewY = origGrid.y(i, 0) + forceY;

		int newEdgeId;
		double newX, newY;
		jMinEdge.findMappedPoint(tmpNewX, tmpNewY, edgeId, &newX, &newY, &newEdgeId);
		jMinEdgeIds[i - 1] = newEdgeId;

		grid->x(i, 0) = newX;
		grid->y(i, 0) = newY;

		err = std::max(err, std::abs(newX - origGrid.x(i, 0)) / avrLen);
		err = std::max(err, std::abs(newY - origGrid.y(i, 0)) / avrLen);
	}

	// j = jmax
	auto& jMaxEdge = grid->jMaxEdge();
	auto& jMaxEdgeIds = grid->jMaxEdgeIds();
	int jmax_1 = grid->jMax() - 1;
	for (int i = 1; i < grid->iMax() - 1; ++i) {
		int edgeId = jMaxEdgeIds.at(i - 1);
		QVector2D edgeDir = jMaxEdge.lineDirection(edgeId);

		std::vector<double> dxVec, dyVec, cVec;
		double dx, dy;

		calcDelta(&origGrid, i, jmax_1, i    , jmax_1 - 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, i, jmax_1, i - 1, jmax_1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, i, jmax_1, i + 1, jmax_1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1);

		calcDelta(&origGrid, i, jmax_1, i - 1, jmax_1 - 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		calcDelta(&origGrid, i, jmax_1, i + 1, jmax_1 - 1, &dx, &dy);
		dxVec.push_back(dx); dyVec.push_back(dy); cVec.push_back(1 / ROOT2);

		double forceX, forceY, avrLen;
		calcForce(dxVec, dyVec, cVec, springK, &forceX, &forceY, &avrLen);

		double r = forceX * edgeDir.x() + forceY * edgeDir.y();
		forceX = r * edgeDir.x();
		forceY = r * edgeDir.y();

		double tmpNewX = origGrid.x(i, jmax_1) + forceX;
		double tmpNewY = origGrid.y(i, jmax_1) + forceY;

		int newEdgeId;
		double newX, newY;
		jMaxEdge.findMappedPoint(tmpNewX, tmpNewY, edgeId, &newX, &newY, &newEdgeId);
		jMaxEdgeIds[i - 1] = newEdgeId;

		grid->x(i, jmax_1) = newX;
		grid->y(i, jmax_1) = newY;

		err = std::max(err, std::abs(newX - origGrid.x(i, jmax_1)) / avrLen);
		err = std::max(err, std::abs(newY - origGrid.y(i, jmax_1)) / avrLen);
	}


	return err;
}

} // namespace

void SpringSolver::solve(std::vector<double>* x, std::vector<double>* y, int imax, int jmax, double eps, int maxIter, int* progress, bool* cancel)
{
	int count = 0;
	double err = 0;
	double errLimit = 0.05;
	double springK = 1.2;

	Grid grid(x, y, imax, jmax);
	do {
		++ count;
		err = updateGrid(&grid, springK);
		if (err > errLimit) {
			springK *= 0.5;
		} else {
			springK *= 1.05;
		}

		if (count > maxIter) {break;}
		if (*cancel) {break;}

		*progress = count * 100 / maxIter;

	} while (err > eps);
}

SpringSolver::SpringSolver()
{}
