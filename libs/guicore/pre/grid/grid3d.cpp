#include "grid3d.h"

Grid3D::Grid3D(vtkPointSet* ps, const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(ps, zonename, type, parent)
{}

Grid3D::~Grid3D()
{}
