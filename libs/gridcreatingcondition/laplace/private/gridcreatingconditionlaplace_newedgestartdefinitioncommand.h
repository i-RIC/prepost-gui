#ifndef GRIDCREATINGCONDITIONLAPLACE_NEWEDGESTARTDEFINITIONCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_NEWEDGESTARTDEFINITIONCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class GridCreatingConditionLaplace::NewEdgeStartDefinitionCommand : public QUndoCommand
{
public:
	NewEdgeStartDefinitionCommand(const QPoint& point, GridCreatingConditionLaplace* cond);
	NewEdgeStartDefinitionCommand(const QPointF& pos, GridCreatingConditionLaplace* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	QPointF m_position;
	GridCreatingConditionLaplace* m_condition;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_NEWEDGESTARTDEFINITIONCOMMAND_H
