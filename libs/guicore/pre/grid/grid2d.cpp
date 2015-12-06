#include "grid2d.h"

Grid2D::Grid2D(vtkPointSet* ps, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, type, parent)
{}

Grid2D::Grid2D(vtkPointSet* ps, const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, zonename, type, parent)
{}

Grid2D::~Grid2D()
{}
