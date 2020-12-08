#include "poststringresultdataitem_setsettingcommand.h"

#include <guicore/postcontainer/poststringresult.h>

PostStringResultDataItem::SetSettingCommand::SetSettingCommand(PostStringResult* result, const PostStringResultDataItem::Setting& dispSetting, PostStringResultDataItem* item) :
	QUndoCommand(PostStringResultDataItem::tr("Edit Label Setting")),
	m_newResult {new PostStringResult(nullptr)},
	m_newDispSetting {dispSetting},
	m_oldResult {new PostStringResult(nullptr)},
	m_oldDispSetting {item->m_setting},
	m_item {item}
{
	m_newResult->copyValue(*result);
	m_oldResult->copyValue(*(item->m_stringResult));
}

PostStringResultDataItem::SetSettingCommand::~SetSettingCommand()
{
	delete m_newResult;
	delete m_oldResult;
}

void PostStringResultDataItem::SetSettingCommand::redo()
{
	apply(m_newResult, m_newDispSetting);
}

void PostStringResultDataItem::SetSettingCommand::undo()
{
	apply(m_oldResult, m_oldDispSetting);
}

void PostStringResultDataItem::SetSettingCommand::apply(PostStringResult* result, const PostStringResultDataItem::Setting& dispSetting)
{
	m_item->m_stringResult->copyValue(*result);
	m_item->m_setting = dispSetting;
	m_item->update();
}
