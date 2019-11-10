#ifndef POLYGONMOVEVERTEXCOMMAND_H
#define POLYGONMOVEVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPolygonF>
#include <QUndoCommand>

class PolygonController;

class PolygonMoveVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolygonMoveVertexCommand(bool keyDown, int vertexId, const QPointF& point, PolygonController* polygon);
	PolygonMoveVertexCommand(const QString& title, bool keyDown, int vertexId, const QPointF& point, PolygonController* polygon);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void applyPoint(const QPointF& point);

	bool m_keyDown;
	int m_vertexId;
	QPointF m_newPoint;
	QPointF m_oldPoint;
	PolygonController* m_polygon;
};

#endif // POLYGONMOVEVERTEXCOMMAND_H
