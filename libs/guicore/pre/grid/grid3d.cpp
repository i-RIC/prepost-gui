#include "grid3d.h"

Grid3D::Grid3D(const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent) :
	Grid(zonename, type, parent)
{}

Grid3D::~Grid3D()
{}
