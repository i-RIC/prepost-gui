#ifndef POLYGONREMOVEVERTEXCOMMAND_H
#define POLYGONREMOVEVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QUndoCommand>
#include <QObject>
#include <QPointF>

class PolygonController;

class GUIBASEDLL_EXPORT PolygonRemoveVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolygonRemoveVertexCommand(int vertexId, PolygonController* polygon);
	PolygonRemoveVertexCommand(const QString& title, int vertexId, PolygonController* polygon);

	void redo() override;
	void undo() override;

private:
	int m_vertexId;
	QPointF m_point;
	PolygonController* m_polygon;
};

#endif // POLYGONREMOVEVERTEXCOMMAND_H
