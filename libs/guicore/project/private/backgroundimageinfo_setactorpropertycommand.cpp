#include "backgroundimageinfo_impl.h"
#include "backgroundimageinfo_setactorpropertycommand.h"

BackgroundImageInfo::SetActorPropertyCommand::SetActorPropertyCommand(const BackgroundImageInfo::Setting& newSetting, const BackgroundImageInfo::Setting& oldSetting, BackgroundImageInfo* info) :
	QUndoCommand(QObject::tr("Reallocate Background Image")),
	m_newSetting {newSetting},
	m_oldSetting {oldSetting},
	m_info {info}
{}

void BackgroundImageInfo::SetActorPropertyCommand::redo()
{
	applySetting(m_newSetting);
}

void BackgroundImageInfo::SetActorPropertyCommand::undo()
{
	applySetting(m_oldSetting);
}

void BackgroundImageInfo::SetActorPropertyCommand::applySetting(const BackgroundImageInfo::Setting& setting)
{
	m_info->impl->m_setting = setting;
	m_info->informChange();
}
