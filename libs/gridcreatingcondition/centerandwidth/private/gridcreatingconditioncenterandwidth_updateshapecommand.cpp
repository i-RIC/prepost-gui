#include "gridcreatingconditioncenterandwidth_updateshapecommand.h"

#include <QUndoCommand>

GridCreatingConditionCenterAndWidth::UpdateShapeCommand::UpdateShapeCommand(QUndoCommand* com, GridCreatingConditionCenterAndWidth* item) :
	m_command {com},
	m_item {item}
{}

void GridCreatingConditionCenterAndWidth::UpdateShapeCommand::redo()
{
	m_command->redo();
	m_item->updateShapeData();
}

void GridCreatingConditionCenterAndWidth::UpdateShapeCommand::undo()
{
	m_command->undo();
	m_item->updateShapeData();
}

int GridCreatingConditionCenterAndWidth::UpdateShapeCommand::id() const
{
	return m_command->id();
}

bool GridCreatingConditionCenterAndWidth::UpdateShapeCommand::mergeWith(const QUndoCommand *other)
{
	const UpdateShapeCommand* c = dynamic_cast<const UpdateShapeCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
