#include "geodatapointmap_pointsmanager_addpointscommand.h"

GeoDataPointmap::PointsManager::AddPointsCommand::AddPointsCommand(const std::vector<QPointF>& points, const std::vector<double>& values, PointsManager* manager) :
	QUndoCommand {GeoDataPointmap::tr("Add Points")},
	m_newPoints {vtkSmartPointer<vtkPoints>::New()},
	m_newValues {vtkSmartPointer<vtkDoubleArray>::New()},
	m_oldPoints {manager->points()->GetPoints()},
	m_oldValues {manager->values()},
	m_manager {manager}
{
	m_newPoints->DeepCopy(m_oldPoints.Get());
	m_newValues->DeepCopy(m_oldValues.Get());

	for (int i = 0; i < points.size(); ++i) {
		const auto& p = points.at(i);
		double v = values.at(i);
		m_newPoints->InsertNextPoint(p.x(), p.y(), 0);
		m_newValues->InsertNextValue(v);
	}
}

void GeoDataPointmap::PointsManager::AddPointsCommand::redo()
{
	apply(m_newPoints, m_newValues);
}

void GeoDataPointmap::PointsManager::AddPointsCommand::undo()
{
	apply(m_oldPoints, m_oldValues);
}

void GeoDataPointmap::PointsManager::AddPointsCommand::apply(vtkPoints* points, vtkDoubleArray* vals)
{
	m_manager->setPoints(points, vals);
}
