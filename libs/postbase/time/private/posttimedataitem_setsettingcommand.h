#ifndef POSTTIMEDATAITEM_SETSETTINGCOMMAND_H
#define POSTTIMEDATAITEM_SETSETTINGCOMMAND_H

#include "../posttimedataitem.h"

#include <QUndoCommand>

class PostTimeDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const PostTimeDataItem::Setting& setting, PostTimeDataItem* item);

	void redo() override;
	void undo() override;

private:
	void applySetting(const PostTimeDataItem::Setting& s, bool enabled);

	PostTimeDataItem::Setting m_newSetting;

	PostTimeDataItem::Setting m_oldSetting;
	bool m_oldEnabled;

	PostTimeDataItem* m_item;
};

#endif // POSTTIMEDATAITEM_SETSETTINGCOMMAND_H
