#ifndef GRIDCREATINGCONDITIONTRIANGLE_REMOVEPOLYGONVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_REMOVEPOLYGONVERTEXCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

#include <vtkType.h>

class GridCreatingConditionTriangle::RemovePolygonVertexCommand : public QUndoCommand
{
public:
	RemovePolygonVertexCommand(vtkIdType vertexId, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;

private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_REMOVEPOLYGONVERTEXCOMMAND_H
