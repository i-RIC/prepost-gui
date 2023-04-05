#include "geodatapointmap_pointsmanager_editsinglepointcommand.h"

#include <misc/qundocommandhelper.h>

GeoDataPointmap::PointsManager::EditSinglePointCommand::EditSinglePointCommand(bool apply, double x, double y, double val, vtkIdType index, PointsManager* manager) :
	m_apply {apply},
	m_newPoint {x, y},
	m_newValue {val},
	m_index {index},
	m_manager {manager}
{
	double v[3];
	manager->points()->GetPoint(index, v);
	m_oldPoint = QPointF(v[0], v[1]);

	m_oldValue = manager->values()->GetValue(index);
}

void GeoDataPointmap::PointsManager::EditSinglePointCommand::redo()
{
	apply(m_newPoint, m_newValue);
}

void GeoDataPointmap::PointsManager::EditSinglePointCommand::undo()
{
	apply(m_oldPoint, m_oldValue);
}

int GeoDataPointmap::PointsManager::EditSinglePointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPointmap::PointsManager::EditSinglePointCommand");
}

bool GeoDataPointmap::PointsManager::EditSinglePointCommand::mergeWith(const QUndoCommand* other)
{
	auto command = dynamic_cast<const EditSinglePointCommand*> (other);
	if (command == nullptr) {return false;}
	if (m_apply == false) {return false;}
	if (m_index != command->m_index) {return false;}

	m_newPoint = command->m_newPoint;
	m_newValue = command->m_newValue;
	m_apply = command->m_apply;
	return true;
}

void GeoDataPointmap::PointsManager::EditSinglePointCommand::apply(const QPointF& p, double v)
{
	auto points = m_manager->points()->GetPoints();
	points->SetPoint(m_index, p.x(), p.y(), 0);
	points->Modified();

	auto values = m_manager->values();
	values->SetValue(m_index, v);
	values->Modified();
}
