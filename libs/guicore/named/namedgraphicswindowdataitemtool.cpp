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
		// auto model = namedItem->standardItem()->model();
		if (noSignal) {
			namedItem->setIsCommandExecuting(true);
			// if (model != nullptr) {model->blockSignals(true);}
		}
		namedItem->standardItem()->setCheckState(Qt::Unchecked);
		if (noSignal) {
			namedItem->setIsCommandExecuting(false);
			// if (model != nullptr) {model->blockSignals(false);}
		}
	}
	if (name == "") {return true;}

	if (itemToCheck == nullptr) {return false;}

	// auto model = itemToCheck->standardItem()->model();
	if (noSignal) {
		itemToCheck->setIsCommandExecuting(true);
		// if (model != nullptr) {model->blockSignals(true);}
	}
	itemToCheck->standardItem()->setCheckState(Qt::Checked);
	if (noSignal) {
		itemToCheck->setIsCommandExecuting(false);
		// if (model != nullptr) {model->blockSignals(false);}
	}
	return true;
}
