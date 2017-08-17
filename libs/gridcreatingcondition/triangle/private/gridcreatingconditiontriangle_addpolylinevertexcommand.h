#ifndef GRIDCREATINGCONDITIONTRIANGLE_ADDPOLYLINEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_ADDPOLYLINEVERTEXCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

#include <vtkType.h>

class GridCreatingConditionTriangle::AddPolyLineVertexCommand : public QUndoCommand
{
public:
	AddPolyLineVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_ADDPOLYLINEVERTEXCOMMAND_H
