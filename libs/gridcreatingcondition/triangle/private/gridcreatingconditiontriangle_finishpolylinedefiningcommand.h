#ifndef GRIDCREATINGCONDITIONTRIANGLE_FINISHPOLYLINEDEFININGCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_FINISHPOLYLINEDEFININGCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>

class GridCreatingConditionTriangle::FinishPolyLineDefiningCommand : public QUndoCommand
{
public:
	FinishPolyLineDefiningCommand(GridCreatingConditionTriangle* polygon);

	void redo() override;
	void undo() override;

private:
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractLine* m_targetLine;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_FINISHPOLYLINEDEFININGCOMMAND_H
