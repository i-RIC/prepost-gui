#include "grid2d.h"

Grid2D::Grid2D(SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(type, parent)
{}

Grid2D::Grid2D(const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(zonename, type, parent)
{}

Grid2D::~Grid2D()
{}
