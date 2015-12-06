#include "targeteditemi.h"
#include "targeteditemsettargetcommand.h"

TargetedItemSetTargetCommand::TargetedItemSetTargetCommand(const std::string& newTarget, TargetedItemI* item, const QString& commandName) :
	QUndoCommand(commandName),
	m_newTarget (newTarget),
	m_oldTarget (item->target()),
	m_item {item}
{}

TargetedItemSetTargetCommand::~TargetedItemSetTargetCommand()
{}

void TargetedItemSetTargetCommand::redo()
{
	m_item->setTarget(m_newTarget);
}

void TargetedItemSetTargetCommand::undo()
{
	m_item->setTarget(m_oldTarget);
}
