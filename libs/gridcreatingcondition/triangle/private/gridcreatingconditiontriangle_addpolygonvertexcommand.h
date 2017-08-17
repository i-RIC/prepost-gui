#ifndef GRIDCREATINGCONDITIONTRIANGLE_ADDPOLYGONVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_ADDPOLYGONVERTEXCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

#include <vtkType.h>

class GridCreatingConditionTriangle::AddPolygonVertexCommand : public QUndoCommand
{
public:
	AddPolygonVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_ADDPOLYGONVERTEXCOMMAND_H
