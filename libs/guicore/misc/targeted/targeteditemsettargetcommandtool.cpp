#include "targeteditemsettargetcommand.h"
#include "targeteditemsettargetcommandtool.h"
#include "../../named/namedgraphicwindowdataitem.h"

#include <QStandardItem>

QUndoCommand* TargetedItemSetTargetCommandTool::buildFromNamedItem(NamedGraphicWindowDataItem* item, TargetedItemI* targetedItem, const QString& comName)
{
	if (item->standardItem()->checkState() == Qt::Checked) {
		return new TargetedItemSetTargetCommand(item->name(), targetedItem, comName);
	}	else {
		return new TargetedItemSetTargetCommand("", targetedItem, comName);
	}
}
