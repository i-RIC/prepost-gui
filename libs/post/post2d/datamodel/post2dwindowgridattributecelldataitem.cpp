#include "post2dwindowgridattributeabstractcellgroupdataitem.h"
#include "post2dwindowgridattributecelldataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "private/post2dwindowgridattributeabstractcelldataitem_propertydialog.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>

Post2dWindowGridAttributeCellDataItem::Post2dWindowGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	Post2dWindowGridAttributeAbstractCellDataItem {cond, parent}
{}

QDialog* Post2dWindowGridAttributeCellDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = dynamic_cast<PropertyDialog*> (Post2dWindowGridAttributeAbstractCellDataItem::propertyDialog(parent));

	auto grid = groupDataItem()->gridDataItem()->grid();

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

QString Post2dWindowGridAttributeCellDataItem::positionCaption() const
{
	return tr("Cell");
}
