#include "preprocessorgridattributeabstractcellgroupdataitem.h"
#include "preprocessorgridattributecelldataitem.h"
#include "private/preprocessorgridattributeabstractcelldataitem_propertydialog.h"
#include "public/preprocessorgriddataitem_selectedcellscontroller.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridAttributeCellDataItem::PreProcessorGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	PreProcessorGridAttributeAbstractCellDataItem {cond, parent}
{}

QDialog* PreProcessorGridAttributeCellDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = dynamic_cast<PreProcessorGridAttributeCellDataItem::PropertyDialog*> (PreProcessorGridAttributeAbstractCellDataItem::propertyDialog(parent));
	auto grid = groupDataItem()->gridDataItem()->grid()->grid();

	if (dynamic_cast<v4Structured2dGrid*>(grid) != nullptr) {
		dialog->hideLineWidth();
	} else {
		auto ugrid = dynamic_cast<v4Structured2dGrid*>(grid);
		if (ugrid != nullptr) {
			auto vgrid = ugrid->vtkConcreteData()->data();
			auto firstCell = vgrid->GetCell(0);
			if (firstCell != nullptr && firstCell->GetCellType() == VTK_TRIANGLE) {
				dialog->hideLineWidth();
			}
		}
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
