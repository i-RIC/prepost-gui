#ifndef POSTSTRINGRESULTDATAITEM_SETSETTINGCOMMAND_H
#define POSTSTRINGRESULTDATAITEM_SETSETTINGCOMMAND_H

#include "../poststringresultdataitem.h"

#include <QUndoCommand>

class PostStringResultDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(PostStringResult* result, PostStringResultDataItem::Setting& dispSetting, PostStringResultDataItem* item);
	~SetSettingCommand();

	void redo() override;
	void undo() override;

private:
	void apply(PostStringResult* result, const PostStringResultDataItem::Setting& dispSetting);

	PostStringResult* m_newResult;
	PostStringResultDataItem::Setting m_newDispSetting;

	PostStringResult* m_oldResult;
	PostStringResultDataItem::Setting m_oldDispSetting;

	PostStringResultDataItem* m_item;
};

#endif // POSTSTRINGRESULTDATAITEM_SETSETTINGCOMMAND_H
