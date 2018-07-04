#ifndef GRIDCREATINGCONDITIONTRIANGLE_EDITPOLYGONCOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_EDITPOLYGONCOORDINATESCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>

class GridCreatingConditionTriangle::EditPolygonCoordinatesCommand : public QUndoCommand
{
public:
	EditPolygonCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GridCreatingConditionTriangle* cond);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	GridCreatingConditionTriangleAbstractPolygon* m_selectedPolygon;
	GridCreatingConditionTriangle* m_condition;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_EDITPOLYGONCOORDINATESCOMMAND_H
