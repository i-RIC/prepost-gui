#include "geodatapointmap_pointsmanager_editpointscommand.h"

#include <misc/qundocommandhelper.h>

GeoDataPointmap::PointsManager::EditPointsCommand::EditPointsCommand(bool apply, double val, const std::vector<vtkIdType>& indices, PointsManager* manager) :
	m_apply {apply},
	m_newValue {val},
	m_indices {indices},
	m_manager {manager}
{
	auto vals = manager->values();
	for (vtkIdType index : indices) {
		m_oldValues.push_back(vals->GetValue(index));
	}
}

void GeoDataPointmap::PointsManager::EditPointsCommand::redo()
{
	auto vals = m_manager->values();
	for (vtkIdType index : m_indices) {
		vals->SetValue(index, m_newValue);
	}
	vals->Modified();
}

void GeoDataPointmap::PointsManager::EditPointsCommand::undo()
{
	auto vals = m_manager->values();
	for (unsigned int i = 0; i < m_indices.size(); ++i) {
		auto index = m_indices.at(i);
		auto val = m_oldValues.at(i);
		vals->SetValue(index, val);
	}
	vals->Modified();
}

int GeoDataPointmap::PointsManager::EditPointsCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPointmap::PointsManager::EditPointsCommand");
}

bool GeoDataPointmap::PointsManager::EditPointsCommand::mergeWith(const QUndoCommand* other)
{
	auto command = dynamic_cast<const EditPointsCommand*> (other);
	if (command == nullptr) {return false;}
	if (m_apply == false) {return false;}
	if (m_indices != command->m_indices) {return false;}

	m_newValue = command->m_newValue;
	m_apply = command->m_apply;
	return true;
}
