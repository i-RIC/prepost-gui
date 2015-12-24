#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_UPDATESHAPECOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_UPDATESHAPECOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>

#include <memory>

class GridCreatingConditionCenterAndWidth::UpdateShapeCommand : public QUndoCommand
{
public:
	UpdateShapeCommand(QUndoCommand* com, GridCreatingConditionCenterAndWidth* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GridCreatingConditionCenterAndWidth* m_item;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_UPDATESHAPECOMMAND_H
