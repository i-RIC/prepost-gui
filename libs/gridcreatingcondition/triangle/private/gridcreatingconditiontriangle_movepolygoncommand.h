#ifndef GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYGONCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYGONCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

class GridCreatingConditionTriangle::MovePolygonCommand : public QUndoCommand
{
public:
	MovePolygonCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
	QVector2D m_offset;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYGONCOMMAND_H
