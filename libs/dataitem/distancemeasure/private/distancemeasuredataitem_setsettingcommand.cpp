#include "distancemeasuredataitem_setsettingcommand.h"
#include "distancemeasuredataitem_impl.h"

#include <QStandardItem>

DistanceMeasureDataItem::SetSettingCommand::SetSettingCommand(const QString& name, const DistanceMeasureSetting& setting, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Edit Distance Measure Property")),
	m_newName {name},
	m_newSetting {setting},
	m_oldName {item->standardItem()->text()},
	m_oldSetting{ item->impl->m_setting },
	m_item {item}
{}

void DistanceMeasureDataItem::SetSettingCommand::redo()
{
	m_item->m_isCommandExecuting = true;
	m_item->standardItem()->setText(m_newName);
	m_item->m_isCommandExecuting = false;
	m_item->impl->m_setting = m_newSetting;

	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}

void DistanceMeasureDataItem::SetSettingCommand::undo()
{
	m_item->m_isCommandExecuting = true;
	m_item->standardItem()->setText(m_oldName);
	m_item->m_isCommandExecuting = false;
	m_item->impl->m_setting = m_oldSetting;

	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}
