#include "preprocessorgridattributeabstractcellgroupdataitem.h"
#include "preprocessorgridattributejedgedataitem.h"
#include "public/preprocessorgriddataitem_selectedjedgescontroller.h"

PreProcessorGridAttributeJEdgeDataItem::PreProcessorGridAttributeJEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	PreProcessorGridAttributeAbstractCellDataItem {cond, parent}
{}

PreProcessorGridDataItem::SelectedDataWithIdController* PreProcessorGridAttributeJEdgeDataItem::selectedDataController() const
{
	return groupDataItem()->gridDataItem()->selectedJEdgesController();
}

QString PreProcessorGridAttributeJEdgeDataItem::positionCaption() const
{
	return tr("Edge (J-direction)");
}
