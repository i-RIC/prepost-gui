#include "targetedqstringitemsettargetcommand.h"
#include "targetedqstringitemsettargetcommandtool.h"
#include "../../named/namedqstringgraphicwindowdataitem.h"

#include <QStandardItem>

QUndoCommand* TargetedQStringItemSetTargetCommandTool::buildFromNamedItem(NamedQStringGraphicWindowDataItem* item, TargetedQStringItemI* targetedItem, const QString& comName)
{
	if (item->standardItem()->checkState() == Qt::Checked) {
		return new TargetedQStringItemSetTargetCommand(item->name(), targetedItem, comName);
	}	else {
		return new TargetedQStringItemSetTargetCommand("", targetedItem, comName);
	}
}
