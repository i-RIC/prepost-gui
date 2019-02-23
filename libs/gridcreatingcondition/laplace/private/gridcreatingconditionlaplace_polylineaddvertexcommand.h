#ifndef GRIDCREATINGCONDITIONLAPLACE_POLYLINEADDVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_POLYLINEADDVERTEXCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GridCreatingConditionLaplace::PolyLineAddVertexCommand : public QUndoCommand
{
public:
	PolyLineAddVertexCommand(bool keyDown, PolyLineController* line, int edgeId, QPoint point, GridCreatingConditionLaplace* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	int m_vertexId;
	QPointF m_position;
	PolyLineController* m_polyline;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_POLYLINEADDVERTEXCOMMAND_H
