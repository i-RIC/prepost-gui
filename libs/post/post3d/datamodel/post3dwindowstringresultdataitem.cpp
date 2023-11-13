#include "post3dwindowstringresultdataitem.h"

#include <guicore/post/postzonedataitem.h>

Post3dWindowStringResultDataItem::Post3dWindowStringResultDataItem(GraphicsWindowDataItem* parent) :
	PostStringResultDataItem(parent)
{}

PostZoneDataItem* Post3dWindowStringResultDataItem::zoneDataItem() const
{
	return dynamic_cast<PostZoneDataItem*> (parent());
}
