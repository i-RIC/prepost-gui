#ifndef GRIDCREATINGCONDITIONPOISSON_DEFINENEWPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_DEFINENEWPOINTCOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>
#include <QPointF>

class GridCreatingConditionPoisson::DefineNewPointCommand : public QUndoCommand
{
public:
	DefineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionPoisson* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionPoisson* m_condition;
	QPointF m_newPoint;
};

#endif // GRIDCREATINGCONDITIONPOISSON_DEFINENEWPOINTCOMMAND_H
