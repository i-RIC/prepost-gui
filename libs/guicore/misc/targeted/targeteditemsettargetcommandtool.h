#ifndef TARGETEDITEMSETTARGETCOMMANDTOOL_H
#define TARGETEDITEMSETTARGETCOMMANDTOOL_H

#include "../../guicore_global.h"

class NamedGraphicWindowDataItem;
class QUndoCommand;
class TargetedItemI;

class GUICOREDLL_EXPORT TargetedItemSetTargetCommandTool
{
public:
	static QUndoCommand* buildFromNamedItem(NamedGraphicWindowDataItem* item, TargetedItemI* targetedItem, const QString &comName);

private:
	TargetedItemSetTargetCommandTool();
	~TargetedItemSetTargetCommandTool();
};

#endif // TARGETEDITEMSETTARGETCOMMANDTOOL_H
