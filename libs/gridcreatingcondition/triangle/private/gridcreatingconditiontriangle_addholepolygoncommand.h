#ifndef GRIDCREATINGCONDITIONTRIANGLE_ADDHOLEPOLYGONCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_ADDHOLEPOLYGONCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>

class GridCreatingConditionTriangle::AddHolePolygonCommand : public QUndoCommand
{
public:
	AddHolePolygonCommand(GridCreatingConditionTriangleHolePolygon* newPoly, GridCreatingConditionTriangle* pol);
	~AddHolePolygonCommand();
	void redo() override;
	void undo() override;

private:
	bool m_undoed;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleHolePolygon* m_targetPolygon;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_ADDHOLEPOLYGONCOMMAND_H
