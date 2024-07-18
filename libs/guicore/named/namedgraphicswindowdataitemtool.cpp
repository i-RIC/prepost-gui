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
			namedItem->standardItem()->model()->blockSignals(true);
		}
		namedItem->standardItem()->setCheckState(Qt::Unchecked);
		if (noSignal) {
			namedItem->setIsCommandExecuting(false);
			namedItem->standardItem()->model()->blockSignals(false);
		}
	}
	if (name == "") {return true;}

	if (itemToCheck == nullptr) {return false;}

	if (noSignal) {
		itemToCheck->setIsCommandExecuting(true);
		itemToCheck->standardItem()->model()->blockSignals(true);
	}
	itemToCheck->standardItem()->setCheckState(Qt::Checked);
	if (noSignal) {
		itemToCheck->setIsCommandExecuting(false);
		itemToCheck->standardItem()->model()->blockSignals(false);
	}
	return true;
}
