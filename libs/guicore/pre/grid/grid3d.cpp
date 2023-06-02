#include "grid3d.h"

#include <vtkStaticPointLocator.h>

Grid3D::Grid3D(vtkPointSet* ps, const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, zonename, type, parent)
{
	setPointLocator(vtkStaticPointLocator::New());
}

Grid3D::~Grid3D()
{}
