#ifndef POST2DWINDOWINPUTGRIDDATAITEM_IMPL_H
#define POST2DWINDOWINPUTGRIDDATAITEM_IMPL_H

#include "../post2dwindowinputgriddataitem.h"

class Post2dWindowGridAttributeCellGroupDataItem;
class Post2dWindowGridAttributeIEdgeGroupDataItem;
class Post2dWindowGridAttributeJEdgeGroupDataItem;
class Post2dWindowGridAttributeNodeGroupDataItem;
class Post2dWindowBCGroupDataItem;

class Post2dWindowInputGridDataItem::Impl
{
public:
	Impl();
	~Impl();

	Post2dWindowGridShapeDataItem* m_gridShapeDataItem;
	Post2dWindowGridAttributeNodeGroupDataItem* m_nodeGroupDataItem;
	Post2dWindowGridAttributeCellGroupDataItem* m_cellGroupDataItem;
	Post2dWindowGridAttributeIEdgeGroupDataItem* m_iEdgeGroupDataItem;
	Post2dWindowGridAttributeJEdgeGroupDataItem* m_jEdgeGroupDataItem;
	Post2dWindowBCGroupDataItem* m_bcGroupDataItem;
};

#endif // POST2DWINDOWINPUTGRIDDATAITEM_IMPL_H
