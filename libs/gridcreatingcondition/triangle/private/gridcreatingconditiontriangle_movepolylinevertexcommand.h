#ifndef GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYLINEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYLINEVERTEXCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

#include <vtkType.h>

class GridCreatingConditionTriangle::MovePolyLineVertexCommand : public QUndoCommand
{
public:
	MovePolyLineVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
	QVector2D m_offset;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYLINEVERTEXCOMMAND_H
