#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_DEFINENEWPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_DEFINENEWPOINTCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>
#include <QVector2D>

class GridCreatingConditionCenterAndWidth::DefineNewPointCommand : public QUndoCommand
{
public:
	DefineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionCenterAndWidth* m_condition;
	QVector2D m_newPoint;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_DEFINENEWPOINTCOMMAND_H
