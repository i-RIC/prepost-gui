#ifndef SETSETTINGCOMMAND_H
#define SETSETTINGCOMMAND_H

#include "../posttitledataitem.h"

#include <QUndoCommand>

class PostTitleDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(bool enable, const PostTitleSetting& setting, PostTitleDataItem* item);

	void redo() override;
	void undo() override;

private:
	bool m_newEnabled;
	PostTitleSetting m_newSetting;

	bool m_oldEnabled;
	PostTitleSetting m_oldSetting;

	PostTitleDataItem* m_item;
};

#endif // SETSETTINGCOMMAND_H
