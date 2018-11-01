#ifndef GEODATARIVERSURVEY_EXPANDCROSSSECTIONCOMMAND_H
#define GEODATARIVERSURVEY_EXPANDCROSSSECTIONCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::ExpandCrosssectionCommand : public QUndoCommand
{
public:
	ExpandCrosssectionCommand(bool apply, QList<GeoDataRiverPathPoint*>& points, QList<double> ratios, GeoDataRiverSurvey* rs);
	void redo() override;
	void undo() override;

private:
	bool m_apply;
	QList<GeoDataRiverPathPoint*> m_points;
	QList<GeoDataRiverCrosssection::AltitudeList> m_newCrosssections;
	QList<GeoDataRiverCrosssection::AltitudeList> m_oldCrosssections;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_EXPANDCROSSSECTIONCOMMAND_H
