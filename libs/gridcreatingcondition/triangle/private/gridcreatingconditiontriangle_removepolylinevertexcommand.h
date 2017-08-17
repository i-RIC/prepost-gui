#ifndef GRIDCREATINGCONDITIONTRIANGLE_REMOVEPOLYLINEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_REMOVEPOLYLINEVERTEXCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

#include <vtkType.h>

class GridCreatingConditionTriangle::RemovePolyLineVertexCommand : public QUndoCommand
{
public:
	RemovePolyLineVertexCommand(vtkIdType vertexId, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;

private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
};
#endif // GRIDCREATINGCONDITIONTRIANGLE_REMOVEPOLYLINEVERTEXCOMMAND_H
