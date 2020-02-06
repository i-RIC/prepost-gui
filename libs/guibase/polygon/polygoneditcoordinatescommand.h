#ifndef POLYGONEDITCOORDINATESCOMMAND_H
#define POLYGONEDITCOORDINATESCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPolygonF>
#include <QUndoCommand>

class PolygonController;

class PolygonEditCoordinatesCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolygonEditCoordinatesCommand(const std::vector<QPointF>& coords, PolygonController* polygon);
	PolygonEditCoordinatesCommand(const QString& title, const std::vector<QPointF>& coords, PolygonController* polygon);

	void redo() override;
	void undo() override;

private:
	QPolygonF m_newPolylgon;
	QPolygonF m_oldPolygon;

	PolygonController* m_polygon;
};

#endif // POLYGONEDITCOORDINATESCOMMAND_H
