#ifndef GRIDCREATINGCONDITIONTRIANGLE_ADDDIVISIONLINECOMMAND_H
#define GRIDCREATINGCONDITIONTRIANGLE_ADDDIVISIONLINECOMMAND_H

#include "../gridcreatingconditiontriangle.h"

#include <QUndoCommand>

class GridCreatingConditionTriangle::AddDivisionLineCommand : public QUndoCommand
{
public:
	AddDivisionLineCommand(GridCreatingConditionTriangleDivisionLine* newLine, GridCreatingConditionTriangle* pol);
	~AddDivisionLineCommand();

	void redo() override;
	void undo() override;

private:
	bool m_undoed;
	GridCreatingConditionTriangle* m_polygon;
	GridCreatingConditionTriangleDivisionLine* m_targetLine;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_ADDDIVISIONLINECOMMAND_H
