#ifndef GEODATARIVERSURVEY_ROTATERIVERCROSSSECTIONCOMMAND_H
#define GEODATARIVERSURVEY_ROTATERIVERCROSSSECTIONCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::RotateRiverCrosssectionCommand : public QUndoCommand
{
public:
	RotateRiverCrosssectionCommand(bool apply, double angle, GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	QVector2D m_newDirection;
	QVector2D m_oldDirection;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
};


#endif // GEODATARIVERSURVEY_ROTATERIVERCROSSSECTIONCOMMAND_H
