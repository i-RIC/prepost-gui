#include "backgroundcolorediti_setbackgroundcolorcommand.h"

BackgroundColorEditI::SetBackgroundColorCommand::SetBackgroundColorCommand(const QColor color, BackgroundColorEditI* target):
	m_newColor {color},
	m_oldColor {target->backgroundColor()},
	m_target {target}
{}

void BackgroundColorEditI::SetBackgroundColorCommand::redo()
{
	m_target->setBackgroundColor(m_newColor);
}

void BackgroundColorEditI::SetBackgroundColorCommand::undo()
{
	m_target->setBackgroundColor(m_oldColor);
}
