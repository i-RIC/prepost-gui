#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowstringresultdataitem.h"
#include "post2dwindowzonedataitem.h"

Post2dWindowStringResultDataItem::Post2dWindowStringResultDataItem(GraphicsWindowDataItem* parent) :
	PostStringResultDataItem(parent)
{}

PostZoneDataItem* Post2dWindowStringResultDataItem::zoneDataItem() const
{
	auto resultDataItem = dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent());
	return resultDataItem->zoneDataItem();
}
