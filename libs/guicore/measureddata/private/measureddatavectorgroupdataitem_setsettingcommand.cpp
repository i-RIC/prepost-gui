#include "measureddatavectorgroupdataitem_setsettingcommand.h"

#include <misc/stringtool.h>

MeasuredDataVectorGroupDataItem::SetSettingCommand::SetSettingCommand(const MeasuredDataVectorSetting& setting, MeasuredDataVectorGroupDataItem* item) :
	QUndoCommand {MeasuredDataVectorGroupDataItem::tr("Update Arrow Setting")},
	m_newSetting {setting},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void MeasuredDataVectorGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newSetting);
}

void MeasuredDataVectorGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldSetting);
}

void MeasuredDataVectorGroupDataItem::SetSettingCommand::applySetting(const MeasuredDataVectorSetting& s)
{
	m_item->m_setting = s;
	m_item->setTarget(iRIC::toStr(s.target));
	m_item->updateActorSettings();
}
