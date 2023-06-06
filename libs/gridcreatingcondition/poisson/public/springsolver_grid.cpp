#include "springsolver_grid.h"

namespace {

int index(int i, int j, int imax)
{
	return i + j * imax;
}

} // namespace

SpringSolver::Grid::Grid(std::vector<double>* x, std::vector<double>* y, int imax, int jmax) :
	m_x {x},
	m_y {y},
	m_iMax {imax},
	m_jMax {jmax}
{
	std::vector<double> edgeX, edgeY;

	// jMin
	edgeX.clear();
	edgeY.clear();
	for (int i = 0; i < m_iMax; ++i) {
		edgeX.push_back(this->x(i, 0));
		edgeY.push_back(this->y(i, 0));
	}
	m_jMinEdge.setXY(edgeX, edgeY);

	// jMax
	edgeX.clear();
	edgeY.clear();
	for (int i = 0; i < m_iMax; ++i) {
		edgeX.push_back(this->x(i, jmax - 1));
		edgeY.push_back(this->y(i, jmax - 1));
	}
	m_jMaxEdge.setXY(edgeX, edgeY);

	// iMin
	edgeX.clear();
	edgeY.clear();
	for (int j = 0; j < m_jMax; ++j) {
		edgeX.push_back(this->x(0, j));
		edgeY.push_back(this->y(0, j));
	}
	m_iMinEdge.setXY(edgeX, edgeY);

	// iMax
	edgeX.clear();
	edgeY.clear();
	for (int j = 0; j < m_jMax; ++j) {
		edgeX.push_back(this->x(imax - 1, j));
		edgeY.push_back(this->y(imax - 1, j));
	}
	m_iMaxEdge.setXY(edgeX, edgeY);

	for (int i = 0; i < m_iMax - 2; ++i) {
		m_jMinEdgeIds.push_back(i);
		m_jMaxEdgeIds.push_back(i);
	}
	for (int j = 0; j < m_jMax - 2; ++j) {
		m_iMinEdgeIds.push_back(j);
		m_iMaxEdgeIds.push_back(j);
	}
}

double& SpringSolver::Grid::x(int i, int j)
{
	return (*m_x)[index(i, j, m_iMax)];
}

double& SpringSolver::Grid::y(int i, int j)
{
	return (*m_y)[index(i, j, m_iMax)];
}

int SpringSolver::Grid::iMax() const
{
	return m_iMax;
}

int SpringSolver::Grid::jMax() const
{
	return m_jMax;
}

std::vector<double>* SpringSolver::Grid::xVec() const
{
	return m_x;
}

std::vector<double>* SpringSolver::Grid::yVec() const
{
	return m_y;
}

SpringSolver::Edge& SpringSolver::Grid::iMinEdge()
{
	return m_iMinEdge;
}

SpringSolver::Edge& SpringSolver::Grid::iMaxEdge()
{
	return m_iMaxEdge;
}

SpringSolver::Edge& SpringSolver::Grid::jMinEdge()
{
	return m_jMinEdge;
}

SpringSolver::Edge& SpringSolver::Grid::jMaxEdge()
{
	return m_jMaxEdge;
}

std::vector<int>& SpringSolver::Grid::iMinEdgeIds()
{
	return m_iMinEdgeIds;
}

std::vector<int>& SpringSolver::Grid::iMaxEdgeIds()
{
	return m_iMaxEdgeIds;
}

std::vector<int>& SpringSolver::Grid::jMinEdgeIds()
{
	return m_jMinEdgeIds;
}

std::vector<int>& SpringSolver::Grid::jMaxEdgeIds()
{
	return m_jMaxEdgeIds;
}
