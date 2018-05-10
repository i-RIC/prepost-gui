#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_SWITCHSTATUSCOMMAND_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_SWITCHSTATUSCOMMAND_H

#include "../gridcreatingconditioncompoundchannel.h"

#include <QUndoCommand>

class GridCreatingConditionCompoundChannel::SwitchStatusCommand : public QUndoCommand
{
public:
	SwitchStatusCommand(GridCreatingConditionCompoundChannel::Status newStatus, GridCreatingConditionCompoundChannel* condition);

	void redo() override;
	void undo() override;

private:
	GridCreatingConditionCompoundChannel::Status m_newStatus;
	GridCreatingConditionCompoundChannel* m_condition;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_SWITCHSTATUSCOMMAND_H
