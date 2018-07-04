#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_EDITCOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_EDITCOORDINATESCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>

class GridCreatingConditionCenterAndWidth::EditCoordinatesCommand : public QUndoCommand
{
public:
	EditCoordinatesCommand(const std::vector<QPointF>& coords, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

private:
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_EDITCOORDINATESCOMMAND_H
