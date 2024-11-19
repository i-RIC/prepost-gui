#ifndef V4STRUCTURED1DGRID_IMPL_H
#define V4STRUCTURED1DGRID_IMPL_H

#include "../v4structured1dgrid.h"
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>

class v4Structured1dGrid::Impl
{
public:
	Impl(v4Structured1dGrid* parent);
	~Impl();

	void setupVtkObjects();

	vtkIdType m_dimension;

private:
	v4Structured1dGrid* m_parent;
};

#endif // V4STRUCTURED1DGRID_IMPL_H
