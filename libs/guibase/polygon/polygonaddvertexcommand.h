#ifndef POLYGONADDVERTEXCOMMAND_H
#define POLYGONADDVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class PolygonController;

class GUIBASEDLL_EXPORT PolygonAddVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolygonAddVertexCommand(bool keyDown, int edgeId, const QPointF& point, PolygonController* polygon);
	PolygonAddVertexCommand(const QString& title, bool keyDown, int edgeId, const QPointF& point, PolygonController* polygon);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	int m_vertexId;
	QPointF m_point;
	PolygonController* m_polygon;
};

#endif // POLYGONADDVERTEXCOMMAND_H
