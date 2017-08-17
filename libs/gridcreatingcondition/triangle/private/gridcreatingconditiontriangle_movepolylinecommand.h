#ifndef GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYLINECOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYLINECOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

class GridCreatingConditionTriangle::MovePolyLineCommand : public QUndoCommand
{
public:
	MovePolyLineCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
	QVector2D m_offset;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_MOVEPOLYLINECOMMAND_H
