#include "backgroundcoloreditinterface_setbackgroundcolorcommand.h"

BackgroundColorEditInterface::SetBackgroundColorCommand::SetBackgroundColorCommand(const QColor color, BackgroundColorEditInterface* target):
	m_newColor {color},
	m_oldColor {target->backgroundColor()},
	m_target {target}
{}

void BackgroundColorEditInterface::SetBackgroundColorCommand::redo()
{
	m_target->setBackgroundColor(m_newColor);
}

void BackgroundColorEditInterface::SetBackgroundColorCommand::undo()
{
	m_target->setBackgroundColor(m_oldColor);
}
