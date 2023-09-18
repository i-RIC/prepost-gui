#ifndef POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_IMPL_H
#define POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_IMPL_H

#include "../post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowgridshapedataitem_setting.h"

class Post2dBirdEyeWindowGridShapeDataItem::Impl
{
public:
	Impl();
	~Impl();

	Setting m_setting;

	vtkWarpScalar* m_gridWarp;
	vtkWarpScalar* m_labelWarp;
};

#endif // POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_IMPL_H
