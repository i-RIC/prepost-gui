#ifndef GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYGONVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYGONVERTEXCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

#include <vtkType.h>

class GridCreatingConditionTriangle::MovePolygonVertexCommand : public QUndoCommand
{
public:
	MovePolygonVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
	QVector2D m_offset;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYGONVERTEXCOMMAND_H
