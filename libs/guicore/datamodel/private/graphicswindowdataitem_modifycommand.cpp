#include "graphicswindowdataitem_modifycommand.h"
#include "../../project/projectmainfile.h"

GraphicsWindowDataItem::ModifyCommand::ModifyCommand(QUndoCommand *child, ProjectMainFile* main) :
	m_command {child},
	m_mainFile {main},
	m_wasModified {main->isModified()}
{}

GraphicsWindowDataItem::ModifyCommand::~ModifyCommand()
{}

void GraphicsWindowDataItem::ModifyCommand::redo()
{
	m_command.get()->redo();
	m_mainFile->setModified();
}

void GraphicsWindowDataItem::ModifyCommand::undo()
{
	m_command.get()->undo();
	m_mainFile->setModified(m_wasModified);
}

int GraphicsWindowDataItem::ModifyCommand::id() const
{
	return m_command->id();
}

bool GraphicsWindowDataItem::ModifyCommand::mergeWith(const QUndoCommand *other)
{
	const ModifyCommand* modc = dynamic_cast<const ModifyCommand*> (other);
	if (modc == nullptr) {return false;}

	return m_command.get()->mergeWith(modc->m_command.get());
}
