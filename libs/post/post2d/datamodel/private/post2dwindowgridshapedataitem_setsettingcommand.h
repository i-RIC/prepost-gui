#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowgridshapedataitem.h"

#include <QUndoCommand>

class Post2dWindowGridShapeDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const GridShapeEditDialog::Setting& setting, Post2dWindowGridShapeDataItem* item);

	void redo() override;
	void undo() override;

private:
	GridShapeEditDialog::Setting m_newSetting;
	GridShapeEditDialog::Setting m_oldSetting;
	bool m_oldEnabled;

	Post2dWindowGridShapeDataItem* m_item;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_SETSETTINGCOMMAND_H
