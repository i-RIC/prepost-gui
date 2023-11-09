#include "post2dwindowgridattributeabstractcellgroupdataitem.h"
#include "post2dwindowgridattributecelldataitem.h"
#include "private/post2dwindowgridattributeabstractcelldataitem_propertydialog.h"

Post2dWindowGridAttributeCellDataItem::Post2dWindowGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	Post2dWindowGridAttributeAbstractCellDataItem {cond, parent}
{}

QDialog* Post2dWindowGridAttributeCellDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = dynamic_cast<PropertyDialog*> (Post2dWindowGridAttributeAbstractCellDataItem::propertyDialog(parent));
	dialog->hideLineWidth();

	return dialog;
}

QString Post2dWindowGridAttributeCellDataItem::positionCaption() const
{
	return tr("Cell");
}
