#include "targetedqstringitemi.h"
#include "targetedqstringitemsettargetcommand.h"

TargetedQStringItemSetTargetCommand::TargetedQStringItemSetTargetCommand(const QString& newTarget, TargetedQStringItemI* item, const QString& commandName) :
	QUndoCommand(commandName),
	m_newTarget (newTarget),
	m_oldTarget (item->target()),
	m_item {item}
{}

TargetedQStringItemSetTargetCommand::~TargetedQStringItemSetTargetCommand()
{}

void TargetedQStringItemSetTargetCommand::redo()
{
	m_item->setTarget(m_newTarget);
}

void TargetedQStringItemSetTargetCommand::undo()
{
	m_item->setTarget(m_oldTarget);
}
