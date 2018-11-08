#ifndef GEODATARIVERSURVEY_MOVERIVERPATHPOINTCOMMAND_H
#define GEODATARIVERSURVEY_MOVERIVERPATHPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::MoveRiverPathPointCommand : public QUndoCommand
{
public:
	MoveRiverPathPointCommand(bool apply, QVector2D offset, GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	QList<GeoDataRiverPathPoint*> m_points;
	QList<QVector2D> m_oldPositions;
	QList<QVector2D> m_newPositions;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_MOVERIVERPATHPOINTCOMMAND_H
