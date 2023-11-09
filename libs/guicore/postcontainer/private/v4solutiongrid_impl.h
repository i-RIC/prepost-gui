#ifndef V4SOLUTIONGRID_IMPL_H
#define V4SOLUTIONGRID_IMPL_H

#include "../v4solutiongrid.h"

class v4SolutionGrid::Impl
{
public:
	Impl(SolverDefinitionGridType* gridType, v4Grid* grid);

	vtkDataSetAttributes* attributes(Position pos);

	SolverDefinitionGridType* m_gridType;
	v4Grid* m_grid;
};

#endif // V4SOLUTIONGRID_IMPL_H
