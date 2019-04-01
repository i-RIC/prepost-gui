#ifndef GRIDCREATINGCONDITIONLAPLACE_POLYLINEEDITCOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_POLYLINEEDITCOORDINATESCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GridCreatingConditionLaplace::PolyLineEditCoordinatesCommand : public QUndoCommand
{
public:
	PolyLineEditCoordinatesCommand(const std::vector<QPointF>& coords, PolyLineController* line);

	void redo() override;
	void undo() override;

private:
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	PolyLineController* m_line;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_POLYLINEEDITCOORDINATESCOMMAND_H
