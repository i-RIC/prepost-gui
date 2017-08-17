#ifndef GRIDCREATINGCONDITIONTRIANGLE_ADDREMESHPOLYGONCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_ADDREMESHPOLYGONCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>

class GridCreatingConditionTriangle::AddRemeshPolygonCommand : public QUndoCommand
{
public:
	AddRemeshPolygonCommand(GridCreatingConditionTriangleRemeshPolygon* newPoly, GridCreatingConditionTriangle* pol);
	~AddRemeshPolygonCommand();

	void redo() override;
	void undo() override;

private:
	bool m_undoed;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleRemeshPolygon* m_targetPolygon;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_ADDREMESHPOLYGONCOMMAND_H
