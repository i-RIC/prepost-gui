#ifndef POST2DBIRDEYEWINDOWZONEDATAITEM_IMPL_H
#define POST2DBIRDEYEWINDOWZONEDATAITEM_IMPL_H

#include "../post2dbirdeyewindowzonedataitem.h"

class Post2dBirdEyeWindowZoneDataItem::Impl
{
public:
	Impl(const std::string& zoneName);
	~Impl();

	Post2dBirdEyeWindowGridShapeDataItem* m_shapeDataItem;
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* m_scalarGroupTopDataItem;
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* m_cellScalarGroupTopDataItem;

	std::string m_zoneName;
};

#endif // POST2DBIRDEYEWINDOWZONEDATAITEM_IMPL_H
