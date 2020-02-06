#ifndef POLYGONPUSHVERTEXCOMMAND_H
#define POLYGONPUSHVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class PolygonController;

class PolygonPushVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolygonPushVertexCommand(bool keydown, const QPointF& point, PolygonController* polygon);
	PolygonPushVertexCommand(const QString& title, bool keydown, const QPointF& point, PolygonController* polygon);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	QPointF m_point;
	PolygonController* m_polygon;
};

#endif // POLYGONPUSHVERTEXCOMMAND_H
