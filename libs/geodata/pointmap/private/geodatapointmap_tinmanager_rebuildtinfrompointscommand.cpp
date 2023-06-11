#include "geodatapointmap_tinmanager_rebuildtinfrompointscommand.h"

GeoDataPointmap::TINManager::RebuildTinFromPointsCommand::RebuildTinFromPointsCommand(QUndoCommand* command, TINManager* manager) :
	QUndoCommand {},
	m_command {command},
	m_manager {manager}
{}

void GeoDataPointmap::TINManager::RebuildTinFromPointsCommand::redo()
{
	m_command->redo();
	m_manager->rebuildTinFromPoints(false);
}

void GeoDataPointmap::TINManager::RebuildTinFromPointsCommand::undo()
{
	m_command->undo();
	m_manager->rebuildTinFromPoints(false);
}

int GeoDataPointmap::TINManager::RebuildTinFromPointsCommand::id() const
{
	return m_command->id();
}

bool GeoDataPointmap::TINManager::RebuildTinFromPointsCommand::mergeWith(const QUndoCommand *other)
{
	auto command = dynamic_cast<const RebuildTinFromPointsCommand*> (other);
	if (command == nullptr) {return false;}

	return m_command->mergeWith(command->m_command.get());
}
