#ifndef GRIDCREATINGCONDITIONLAPLACE_CENTERLINESTARTDEFINITIONCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_CENTERLINESTARTDEFINITIONCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class GridCreatingConditionLaplace::CenterLineStartDefinitionCommand : public QUndoCommand
{
public:
	CenterLineStartDefinitionCommand(bool keyDown, const QPoint& point, GridCreatingConditionLaplace* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	QPointF m_position;
	GridCreatingConditionLaplace* m_condition;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_CENTERLINESTARTDEFINITIONCOMMAND_H
