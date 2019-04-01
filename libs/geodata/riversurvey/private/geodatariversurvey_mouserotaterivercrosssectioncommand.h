#ifndef GEODATARIVERSURVEY_MOUSEROTATERIVERCROSSSECTIONCOMMAND_H
#define GEODATARIVERSURVEY_MOUSEROTATERIVERCROSSSECTIONCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand : public QUndoCommand
{
public:
	MouseRotateRiverCrosssectionCommand(QPoint from, QPoint to, GeoDataRiverSurvey* data);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	GeoDataRiverPathPoint* m_point;
	QPointF m_oldDirection;
	QPointF m_newDirection;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_MOUSEROTATERIVERCROSSSECTIONCOMMAND_H
