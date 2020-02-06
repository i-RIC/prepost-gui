#ifndef POLYLINEMOVEVERTEXCOMMAND_H
#define POLYLINEMOVEVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GUIBASEDLL_EXPORT PolyLineMoveVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolyLineMoveVertexCommand(bool keyDown, int vertexId, const QPointF& point, PolyLineController* line);
	PolyLineMoveVertexCommand(const QString& title, bool keyDown, int vertexId, const QPointF& point, PolyLineController* line);

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
	PolyLineController* m_polyLine;
};

#endif // POLYLINEMOVEVERTEXCOMMAND_H
