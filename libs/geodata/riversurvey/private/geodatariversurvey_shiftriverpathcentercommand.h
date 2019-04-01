#ifndef GEODATARIVERSURVEY_SHIFTRIVERPATHCENTERCOMMAND_H
#define GEODATARIVERSURVEY_SHIFTRIVERPATHCENTERCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::ShiftRiverPathCenterCommand : public QUndoCommand
{
public:
	ShiftRiverPathCenterCommand(bool apply, double offset, GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	QList<GeoDataRiverPathPoint*> m_points;
	QList<QPointF> m_oldPositions;
	QList<QPointF> m_newPositions;
	QList<GeoDataRiverCrosssection::AltitudeList> m_oldCrosssections;
	QList<GeoDataRiverCrosssection::AltitudeList> m_newCrosssections;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_SHIFTRIVERPATHCENTERCOMMAND_H
