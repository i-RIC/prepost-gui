#include "grid2d.h"

Grid2D::Grid2D(SolverDefinitionGridType::GridType type, ProjectDataItem * parent)
	: Grid(type, parent)
{}

Grid2D::Grid2D(const QString& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem * parent)
	: Grid(zonename, type, parent)
{}
