#include "namedqstringgraphicswindowdataitemtool.h"
#include "namedqstringgraphicwindowdataitem.h"

#include <QStandardItem>

bool NamedQStringGraphicsWindowDataItemTool::checkItemWithName(const QString& name, const std::vector<GraphicsWindowDataItem*>& items, bool noSignal)
{
	NamedQStringGraphicWindowDataItem* itemToCheck = nullptr;
	for (auto item : items) {
		auto namedItem = dynamic_cast<NamedQStringGraphicWindowDataItem*> (item);
		if (namedItem->name() == name) {
			itemToCheck = namedItem;
		}
		if (noSignal) {
			namedItem->setIsCommandExecuting(true);
		}
		namedItem->standardItem()->setCheckState(Qt::Unchecked);
		if (noSignal) {
			namedItem->setIsCommandExecuting(false);
		}
	}
	if (name == "") {return true;}

	if (itemToCheck == nullptr) {return false;}

	if (noSignal) {
		itemToCheck->setIsCommandExecuting(true);
	}
	itemToCheck->standardItem()->setCheckState(Qt::Checked);
	if (noSignal) {
		itemToCheck->setIsCommandExecuting(false);
	}
	return true;
}
