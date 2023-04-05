#include "geodatapointmap_pointsmanager_deletepointscommand.h"
#include "geodatapointmap_tinmanager.h"

GeoDataPointmap::PointsManager::DeletePointsCommand::DeletePointsCommand(std::unordered_set<vtkIdType> deletedPoints, PointsManager* manager) :
	m_newPoints {vtkSmartPointer<vtkPoints>::New()},
	m_newValues {vtkSmartPointer<vtkDoubleArray>::New()},
	m_oldPoints {manager->m_points->GetPoints()},
	m_oldValues {manager->values()},
	m_deletedPoints {deletedPoints},
	m_manager {manager}
{
	m_newPoints->SetDataTypeToDouble();

	for (vtkIdType i = 0; i < m_oldPoints->GetNumberOfPoints(); ++i) {
		auto it = deletedPoints.find(i);
		if (it != deletedPoints.end()) {continue;}

		m_newPoints->InsertNextPoint(m_oldPoints->GetPoint(i));
		m_newValues->InsertNextValue(m_oldValues->GetValue(i));
	}
}

void GeoDataPointmap::PointsManager::DeletePointsCommand::redo()
{
	m_manager->setPoints(m_newPoints, m_newValues);
	m_manager->m_tinManager->updateBreakLinesOnPointsDelete(m_deletedPoints);
}

void GeoDataPointmap::PointsManager::DeletePointsCommand::undo()
{
	m_manager->setPoints(m_oldPoints, m_oldValues);
	m_manager->m_tinManager->updateBreakLinesOnPointsUndoDelete(m_deletedPoints);
}
