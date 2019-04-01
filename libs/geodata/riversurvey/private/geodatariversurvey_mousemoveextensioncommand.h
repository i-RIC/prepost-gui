#ifndef GEODATARIVERSURVEY_MOUSEMOVEEXTENSIONCOMMAND_H
#define GEODATARIVERSURVEY_MOUSEMOVEEXTENSIONCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::MouseMoveExtensionCommand : public QUndoCommand
{
public:
	MouseMoveExtensionCommand(bool left, QPoint to, GeoDataRiverSurvey* data);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void applyPosition(const QPointF& pos);

	bool m_left;
	QPointF m_oldPosition;
	QPointF m_newPosition;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_MOUSEMOVEEXTENSIONCOMMAND_H
