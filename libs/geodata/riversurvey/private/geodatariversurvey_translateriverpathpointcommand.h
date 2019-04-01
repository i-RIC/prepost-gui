#ifndef GEODATARIVERSURVEY_TRANSLATERIVERPATHPOINTCOMMAND_H
#define GEODATARIVERSURVEY_TRANSLATERIVERPATHPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::TranslateRiverPathPointCommand : public QUndoCommand
{
public:
	TranslateRiverPathPointCommand(QPoint from, QPoint to, GeoDataRiverSurvey* data);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	QList<QPointF> m_newPositions;
	QList<QPointF> m_oldPositions;
	QList<GeoDataRiverPathPoint*> m_points;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_TRANSLATERIVERPATHPOINTCOMMAND_H
