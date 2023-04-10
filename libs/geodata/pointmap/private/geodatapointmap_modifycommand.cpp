#include "geodatapointmap_modifycommand.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

GeoDataPointmap::ModifyCommand::ModifyCommand(QUndoCommand* command, GeoDataPointmap* pointmap) :
	QUndoCommand {command->text()},
	m_oldMapped {pointmap->isMapped()},
	m_oldNeedRebuildTin {pointmap->needRebuildTin()},
	m_command {command},
	m_pointmap {pointmap}
{}

void GeoDataPointmap::ModifyCommand::redo()
{
	m_command->redo();
	m_pointmap->geoDataDataItem()->informValueRangeChange();
	m_pointmap->updateActorSetting();
	m_pointmap->setNeedRebuildTin();
	m_pointmap->setMapped(false);
}

void GeoDataPointmap::ModifyCommand::undo()
{
	m_command->undo();
	m_pointmap->geoDataDataItem()->informValueRangeChange();
	m_pointmap->updateActorSetting();
	m_pointmap->setNeedRebuildTin(m_oldNeedRebuildTin);
	m_pointmap->setMapped(m_oldMapped);
}

int GeoDataPointmap::ModifyCommand::id() const
{
	return m_command->id();
}

bool GeoDataPointmap::ModifyCommand::mergeWith(const QUndoCommand* other)
{
	const auto command = dynamic_cast<const ModifyCommand*> (other);
	if (command == nullptr) {return false;}
	return m_command->mergeWith(command->m_command.get());
}
