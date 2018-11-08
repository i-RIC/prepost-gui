#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_DEFINEPOLYLINENEWPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_DEFINEPOLYLINENEWPOINTCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::DefinePolyLineNewPointCommand : public QUndoCommand
{
public:
	DefinePolyLineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionCompoundChannel* cond);

	void redo() override;
	void undo() override;
	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionCompoundChannelAbstractLine* m_targetLine;
	GridCreatingConditionCompoundChannel* m_condition;
	QPointF m_newPoint;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_DEFINEPOLYLINENEWPOINTCOMMAND_H
