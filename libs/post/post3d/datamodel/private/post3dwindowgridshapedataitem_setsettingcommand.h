#ifndef POST3DWINDOWGRIDSHAPEDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWGRIDSHAPEDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowgridshapedataitem.h"

#include <QUndoCommand>

class Post3dWindowGridShapeDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const GridShapeEditDialog::Setting& setting, Post3dWindowGridShapeDataItem* item);

	void redo() override;
	void undo() override;

private:
	void apply(const GridShapeEditDialog::Setting& setting, bool enabled);

	GridShapeEditDialog::Setting m_newSetting;
	GridShapeEditDialog::Setting m_oldSetting;
	bool m_oldEnabled;

	Post3dWindowGridShapeDataItem* m_item;
};

#endif // POST3DWINDOWGRIDSHAPEDATAITEM_SETSETTINGCOMMAND_H
