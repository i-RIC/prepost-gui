#include "post2dwindowgridattributeabstractcellgroupdataitem.h"
#include "post2dwindowgridattributeiedgedataitem.h"

Post2dWindowGridAttributeIEdgeDataItem::Post2dWindowGridAttributeIEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	Post2dWindowGridAttributeAbstractCellDataItem {cond, parent}
{}

QString Post2dWindowGridAttributeIEdgeDataItem::positionCaption() const
{
	return tr("Edge (I-direction)");
}
