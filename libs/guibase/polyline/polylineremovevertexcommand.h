#ifndef POLYLINEREMOVEVERTEXCOMMAND_H
#define POLYLINEREMOVEVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QUndoCommand>
#include <QObject>
#include <QPointF>

class PolyLineController;

class GUIBASEDLL_EXPORT PolyLineRemoveVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolyLineRemoveVertexCommand(int vertexId, PolyLineController* line);
	PolyLineRemoveVertexCommand(const QString& title, int vertexId, PolyLineController* line);

	void redo() override;
	void undo() override;

private:
	int m_vertexId;
	QPointF m_point;
	PolyLineController* m_polyLine;
};

#endif // POLYLINEREMOVEVERTEXCOMMAND_H
