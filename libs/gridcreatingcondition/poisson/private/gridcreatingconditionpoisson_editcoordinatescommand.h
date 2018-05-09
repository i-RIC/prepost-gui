#ifndef GRIDCREATINGCONDITIONPOISSON_EDITCOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_EDITCOORDINATESCOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>

class PolyLineController;

class GridCreatingConditionPoisson::EditCoordinatesCommand : public QUndoCommand
{
public:
	EditCoordinatesCommand(const QString& lineName, const std::vector<QPointF>& coords, PolyLineController* line);

	void redo() override;
	void undo() override;

private:
	std::vector<QPointF> m_newCoords;
	std::vector<QPointF> m_oldCoords;

	PolyLineController* m_line;
};

#endif // GRIDCREATINGCONDITIONPOISSON_EDITCOORDINATESCOMMAND_H
