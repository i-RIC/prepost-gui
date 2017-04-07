#ifndef GRID_H
#define GRID_H

#include <vector>

#include "edge.h"

class Grid
{
public:
	Grid(std::vector<double>* x, std::vector<double>* y, int imax, int jmax);

	double& x(int i, int j);
	double& y(int i, int j);

	int iMax() const;
	int jMax() const;

	std::vector<double>* xVec() const;
	std::vector<double>* yVec() const;

	Edge& iMinEdge();
	Edge& iMaxEdge();
	Edge& jMinEdge();
	Edge& jMaxEdge();

	std::vector<int>& iMinEdgeIds();
	std::vector<int>& iMaxEdgeIds();
	std::vector<int>& jMinEdgeIds();
	std::vector<int>& jMaxEdgeIds();

private:
	std::vector<double>* m_x;
	std::vector<double>* m_y;

	int m_iMax;
	int m_jMax;

	Edge m_iMinEdge;
	Edge m_iMaxEdge;
	Edge m_jMinEdge;
	Edge m_jMaxEdge;

	std::vector<int> m_iMinEdgeIds;
	std::vector<int> m_iMaxEdgeIds;
	std::vector<int> m_jMinEdgeIds;
	std::vector<int> m_jMaxEdgeIds;
};

#endif // GRID_H
