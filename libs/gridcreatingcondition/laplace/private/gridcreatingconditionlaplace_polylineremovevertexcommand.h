#ifndef GRIDCREATINGCONDITIONLAPLACE_POLYLINEREMOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_POLYLINEREMOVEVERTEXCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GridCreatingConditionLaplace::PolyLineRemoveVertexCommand : public QUndoCommand
{
public:
	PolyLineRemoveVertexCommand(int vertexId, PolyLineController* line);

	void redo() override;
	void undo() override;

private:
	int m_vertexId;
	QPointF m_position;
	PolyLineController* m_polyLine;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_POLYLINEREMOVEVERTEXCOMMAND_H
