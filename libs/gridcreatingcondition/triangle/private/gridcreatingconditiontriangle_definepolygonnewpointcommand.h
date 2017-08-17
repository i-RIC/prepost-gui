#ifndef GRIDCREATINGCONDITIONTRIANGLE_DEFINEPOLYGONNEWPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_DEFINEPOLYGONNEWPOINTCOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>
#include <QVector2D>

class GridCreatingConditionTriangle::DefinePolygonNewPointCommand : public QUndoCommand
{
public:
	DefinePolygonNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionTriangle* pol);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleAbstractPolygon* m_targetPolygon;
	QVector2D m_newPoint;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_DEFINEPOLYGONNEWPOINTCOMMAND_H
