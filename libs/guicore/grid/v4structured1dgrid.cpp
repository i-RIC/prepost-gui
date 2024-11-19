#include "v4structured1dgrid.h"
#include "private/v4structured1dgrid_impl.h"

v4Structured1dGrid::v4Structured1dGrid() :
	v4Grid1dT<vtkStructuredGridExtended2d> {},
	impl {new Impl {this}}
{}

v4Structured1dGrid::~v4Structured1dGrid()
{}

void v4Structured1dGrid::setDimension(vtkIdType dim)
{
	impl->m_dimension = dim;

	impl->setupVtkObjects();
}

vtkIdType v4Structured1dGrid::dimension() const
{
	return impl->m_dimension;
}
