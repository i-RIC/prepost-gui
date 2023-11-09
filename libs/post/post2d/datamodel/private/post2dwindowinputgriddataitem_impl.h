#ifndef POST2DWINDOWINPUTGRIDDATAITEM_IMPL_H
#define POST2DWINDOWINPUTGRIDDATAITEM_IMPL_H

#include "../post2dwindowinputgriddataitem.h"

class Post2dWindowGridAttributeCellGroupDataItem;
class Post2dWindowGridAttributeIEdgeGroupDataItem;
class Post2dWindowGridAttributeJEdgeGroupDataItem;
class Post2dWindowGridAttributeNodeGroupDataItem;

class Post2dWindowInputGridDataItem::Impl
{
public:
	Impl();

	Post2dWindowGridShapeDataItem* m_gridShapeDataItem;
	Post2dWindowGridAttributeNodeGroupDataItem* m_nodeGroupDataItem;
	Post2dWindowGridAttributeCellGroupDataItem* m_cellGroupDataItem;
	Post2dWindowGridAttributeIEdgeGroupDataItem* m_iEdgeGroupDataItem;
	Post2dWindowGridAttributeJEdgeGroupDataItem* m_jEdgeGroupDataItem;
};

#endif // POST2DWINDOWINPUTGRIDDATAITEM_IMPL_H
