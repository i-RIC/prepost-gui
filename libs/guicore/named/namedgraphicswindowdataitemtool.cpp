#include "namedgraphicswindowdataitemtool.h"
#include "namedgraphicwindowdataitem.h"

#include <QStandardItem>

bool NamedGraphicsWindowDataItemTool::checkItemWithName(const std::string& name, const std::vector<GraphicsWindowDataItem*>& items, bool noSignal)
{
	NamedGraphicWindowDataItem* itemToCheck = nullptr;
	for (auto item : items) {
		auto namedItem = dynamic_cast<NamedGraphicWindowDataItem*> (item);
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
