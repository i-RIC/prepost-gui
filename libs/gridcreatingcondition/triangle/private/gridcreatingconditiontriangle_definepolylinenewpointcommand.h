#ifndef GRIDCREATINGCONDITIONTRIANGLE_DEFINEPOLYLINENEWPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_DEFINEPOLYLINENEWPOINTCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

class GridCreatingConditionTriangle::DefinePolyLineNewPointCommand : public QUndoCommand
{
public:
	DefinePolyLineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionTriangle* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_condition;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
	QVector2D m_newPoint;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_DEFINEPOLYLINENEWPOINTCOMMAND_H
