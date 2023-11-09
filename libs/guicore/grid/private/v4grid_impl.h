#ifndef V4GRID_IMPL_H
#define V4GRID_IMPL_H

#include "../v4grid.h"

class v4Grid::Impl
{
public:
	Impl(vtkPointSetExtended* data);
	~Impl();

	vtkPointSetExtended* m_vtkData;
	v4GridStructureCheckerI* m_structureChecker;
	AttributeDataProvider* m_attributeDataProvider;
};

#endif // V4GRID_IMPL_H
