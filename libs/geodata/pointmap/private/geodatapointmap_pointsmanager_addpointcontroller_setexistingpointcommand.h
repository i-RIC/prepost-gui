#ifndef GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTCONTROLLER_SETEXISTINGPOINTCOMMAND_H
#define GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTCONTROLLER_SETEXISTINGPOINTCOMMAND_H

#include "geodatapointmap_pointsmanager_addpointcontroller.h"

#include <QUndoCommand>

class GeoDataPointmap::PointsManager::AddPointController::SetExistingPointCommand : public QUndoCommand
{
public:
	SetExistingPointCommand(vtkIdType pointId, double value, AddPointController* controller);

	void redo() override;
	void undo() override;

private:
	vtkIdType m_pointId;
	double m_value;

	AddPointController* m_controller;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_ADDPOINTCONTROLLER_SETEXISTINGPOINTCOMMAND_H
