#ifndef POLYLINEADDVERTEXCOMMAND_H
#define POLYLINEADDVERTEXCOMMAND_H

#include "../guibase_global.h"

#include <QObject>
#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GUIBASEDLL_EXPORT PolyLineAddVertexCommand : public QObject, public QUndoCommand
{
	Q_OBJECT

public:
	PolyLineAddVertexCommand(bool keyDown, int edgeId, const QPointF& point, PolyLineController* line);
	PolyLineAddVertexCommand(const QString& title, bool keyDown, int edgeId, const QPointF& point, PolyLineController* line);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	int m_vertexId;
	QPointF m_point;
	PolyLineController* m_polyLine;
};

#endif // POLYLINEADDVERTEXCOMMAND_H
