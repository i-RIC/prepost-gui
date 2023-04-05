#ifndef MODIFYCOMMANDANDRENDERDIALOG_H
#define MODIFYCOMMANDANDRENDERDIALOG_H

#include "../guicore_global.h"

#include <misc/modifycommanddialog.h>

class GraphicsWindowDataItem;

class GUICOREDLL_EXPORT ModifyCommandAndRenderDialog : public ModifyCommandDialog
{
public:
	ModifyCommandAndRenderDialog(GraphicsWindowDataItem* item, QWidget* parent);

private:
	void pushCommand(QUndoCommand* command) override;

	GraphicsWindowDataItem* m_item;
};

#endif // MODIFYCOMMANDANDRENDERDIALOG_H
