#include "namedgraphicswindowdataitemtool.h"
#include "namedgraphicwindowdataitem.h"

void NamedGraphicsWindowDataItemTool::checkItemWithName(const std::string& name, const QList<GraphicsWindowDataItem*>& items)
{
	NamedGraphicWindowDataItem* itemToCheck = nullptr;
	for (auto item : items) {
		auto namedItem = dynamic_cast<NamedGraphicWindowDataItem*> (item);
		if (namedItem->name() == name) {
			itemToCheck = namedItem;
		}
		namedItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (itemToCheck == nullptr) {return;}

	itemToCheck->standardItem()->setCheckState(Qt::Checked);
}
