#ifndef GRAPHICSWINDOWDATAITEMUPDATEACTORSETTINGDIALOG_H
#define GRAPHICSWINDOWDATAITEMUPDATEACTORSETTINGDIALOG_H

#include "../guicore_global.h"

#include <misc/modifycommanddialog.h>

class GraphicsWindowDataItem;

class GUICOREDLL_EXPORT GraphicsWindowDataItemUpdateActorSettingDialog : public ModifyCommandDialog
{
public:
	GraphicsWindowDataItemUpdateActorSettingDialog(GraphicsWindowDataItem* item, QWidget* parent);

private:
	void pushCommand(QUndoCommand* command) override;

	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEMUPDATEACTORSETTINGDIALOG_H
