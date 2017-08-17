#ifndef GRIDCREATINGCONDITIONTRIANGLE_FINISHPOLYGONDEFININGCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_FINISHPOLYGONDEFININGCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>

class GridCreatingConditionTriangle::FinishPolygonDefiningCommand : public QUndoCommand
{
public:
	FinishPolygonDefiningCommand(GridCreatingConditionTriangle* polygon);

	void redo() override;
	void undo() override;

private:
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_FINISHPOLYGONDEFININGCOMMAND_H
