#include "preprocessorgridattributeabstractcellgroupdataitem.h"
#include "preprocessorgridattributecelldataitem.h"
#include "private/preprocessorgridattributeabstractcelldataitem_propertydialog.h"
#include "public/preprocessorgriddataitem_selectedcellscontroller.h"

PreProcessorGridAttributeCellDataItem::PreProcessorGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	PreProcessorGridAttributeAbstractCellDataItem {cond, parent}
{}

QDialog* PreProcessorGridAttributeCellDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = PreProcessorGridAttributeAbstractCellDataItem::propertyDialog(parent);
	auto dialog2 = dynamic_cast<PropertyDialog*> (dialog);
	if (dialog2 != nullptr) {
		dialog2->hideLineWidth();
	}

	return dialog;
}

PreProcessorGridDataItem::SelectedDataWithIdController* PreProcessorGridAttributeCellDataItem::selectedDataController() const
{
	return groupDataItem()->gridDataItem()->selectedCellsController();
}

QString PreProcessorGridAttributeCellDataItem::positionCaption() const
{
	return tr("Cell");
}
