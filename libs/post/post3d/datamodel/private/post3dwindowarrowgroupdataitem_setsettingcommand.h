#ifndef POST3DWINDOWARROWGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWARROWGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowarrowgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowArrowGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ArrowSettingContainer& setting, Post3dWindowArrowGroupDataItem* g);

	void redo() override;
	void undo() override;

private:
	bool m_newEnabled;
	ArrowSettingContainer m_newSetting;

	bool m_oldEnabled;
	ArrowSettingContainer m_oldSetting;

	Post3dWindowArrowGroupDataItem* m_item;
};

#endif // POST3DWINDOWARROWGROUPDATAITEM_SETSETTINGCOMMAND_H
