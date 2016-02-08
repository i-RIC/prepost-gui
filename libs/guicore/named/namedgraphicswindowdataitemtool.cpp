#include "namedgraphicswindowdataitemtool.h"
#include "namedgraphicwindowdataitem.h"

bool NamedGraphicsWindowDataItemTool::checkItemWithName(const std::string& name, const QList<GraphicsWindowDataItem*>& items)
{
	NamedGraphicWindowDataItem* itemToCheck = nullptr;
	for (auto item : items) {
		auto namedItem = dynamic_cast<NamedGraphicWindowDataItem*> (item);
		if (namedItem->name() == name) {
			itemToCheck = namedItem;
		}
		namedItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (name == "") {return true;}

	if (itemToCheck == nullptr) {return false;}

	itemToCheck->standardItem()->setCheckState(Qt::Checked);
	return true;
}
