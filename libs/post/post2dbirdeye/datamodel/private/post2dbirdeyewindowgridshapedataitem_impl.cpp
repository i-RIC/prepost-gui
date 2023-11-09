#include "post2dbirdeyewindowgridshapedataitem_impl.h"

#include <vtkWarpScalar.h>

Post2dBirdEyeWindowGridShapeDataItem::Impl::Impl() :
	m_setting {},
	m_gridWarp {vtkWarpScalar::New()},
	m_labelWarp {vtkWarpScalar::New()}
{}

Post2dBirdEyeWindowGridShapeDataItem::Impl::~Impl()
{
	m_gridWarp->Delete();
	m_labelWarp->Delete();
}
