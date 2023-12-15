#ifndef TARGETEDQSTRINGITEMSETTARGETCOMMANDTOOL_H
#define TARGETEDQSTRINGITEMSETTARGETCOMMANDTOOL_H

#include "../../guicore_global.h"

class NamedQStringGraphicWindowDataItem;
class QUndoCommand;
class TargetedQStringItemI;

class GUICOREDLL_EXPORT TargetedQStringItemSetTargetCommandTool
{
public:
	static QUndoCommand* buildFromNamedItem(NamedQStringGraphicWindowDataItem* item, TargetedQStringItemI* targetedItem, const QString &comName);

private:
	TargetedQStringItemSetTargetCommandTool();
	~TargetedQStringItemSetTargetCommandTool();
};

#endif // TARGETEDQSTRINGITEMSETTARGETCOMMANDTOOL_H
