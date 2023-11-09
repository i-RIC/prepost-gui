#include "preprocessorgridattributeabstractcellgroupdataitem.h"
#include "preprocessorgridattributeiedgedataitem.h"
#include "public/preprocessorgriddataitem_selectediedgescontroller.h"

PreProcessorGridAttributeIEdgeDataItem::PreProcessorGridAttributeIEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	PreProcessorGridAttributeAbstractCellDataItem {cond, parent}
{}

PreProcessorGridDataItem::SelectedDataWithIdController* PreProcessorGridAttributeIEdgeDataItem::selectedDataController() const
{
	return groupDataItem()->gridDataItem()->selectedIEdgesController();
}

QString PreProcessorGridAttributeIEdgeDataItem::positionCaption() const
{
	return tr("Edge (I-direction)");
}
