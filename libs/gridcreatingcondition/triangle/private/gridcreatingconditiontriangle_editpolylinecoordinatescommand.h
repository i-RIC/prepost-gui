#ifndef GRIDCREATINGCONDITIONTRIANGLE_EDITPOLYLINECOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_EDITPOLYLINECOORDINATESCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>

class GridCreatingConditionTriangle::EditPolyLineCoordinatesCommand : public QUndoCommand
{
public:
	EditPolyLineCoordinatesCommand(bool apply, std::vector<QPointF> coords, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	GridCreatingConditionTriangleAbstractLine* m_selectedLine;
	GridCreatingConditionTriangle* m_condition;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_EDITPOLYLINECOORDINATESCOMMAND_H
