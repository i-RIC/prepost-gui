#include "grid2d.h"

#include <vtkStaticPointLocator2D.h>

Grid2D::Grid2D(vtkPointSet* ps, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, type, parent)
{
	setPointLocator(vtkStaticPointLocator2D::New());
}

Grid2D::Grid2D(vtkPointSet* ps, const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, zonename, type, parent)
{
	setPointLocator(vtkStaticPointLocator2D::New());
}

Grid2D::~Grid2D()
{}
