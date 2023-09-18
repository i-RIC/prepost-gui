#include "post2dwindowgridattributeabstractcellgroupdataitem.h"
#include "post2dwindowgridattributejedgedataitem.h"

Post2dWindowGridAttributeJEdgeDataItem::Post2dWindowGridAttributeJEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	Post2dWindowGridAttributeAbstractCellDataItem {cond, parent}
{}

QString Post2dWindowGridAttributeJEdgeDataItem::positionCaption() const
{
	return tr("Edge (J-direction)");
}
