#ifndef GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_ADDPOINTCOMMAND_H
#define GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_ADDPOINTCOMMAND_H

#include "geodatapointmap_pointsmanager_interpolatepointcontroller.h"

#include <QUndoCommand>

class GeoDataPointmap::PointsManager::InterpolatePointController::AddPointCommand : public QUndoCommand
{
public:
	AddPointCommand(const QPointF& point, double value, bool newFlag, InterpolatePointController* controller);

	void redo() override;
	void undo() override;

	QPointF m_point;
	double m_value;
	bool m_newFlag;

	InterpolatePointController* m_controller;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_ADDPOINTCOMMAND_H
