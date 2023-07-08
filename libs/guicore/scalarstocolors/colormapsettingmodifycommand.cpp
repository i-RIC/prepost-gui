#include "colormapsettingcontaineri.h"
#include "colormapsettingmodifycommand.h"

#include <misc/qundocommandhelper.h>

ColorMapSettingModifyCommand::ColorMapSettingModifyCommand(ColorMapSettingContainerI* newValue, ColorMapSettingContainerI* target) :
	m_newValue {newValue},
	m_oldValue {target->copy()},
	m_target {target}
{}

ColorMapSettingModifyCommand::~ColorMapSettingModifyCommand()
{
	delete m_newValue;
	delete m_oldValue;
}

void ColorMapSettingModifyCommand::redo()
{
	m_target->copy(*m_newValue);
}

void ColorMapSettingModifyCommand::undo()
{
	m_target->copy(*m_oldValue);
}

int ColorMapSettingModifyCommand::id() const
{
	return iRIC::generateCommandId("ColorMapSettingModifyCommand");
}

bool ColorMapSettingModifyCommand::mergeWith(const QUndoCommand* other)
{
	auto command = dynamic_cast<const ColorMapSettingModifyCommand*> (other);
	if (command == nullptr) {return false;}

	if (m_target != command->m_target) {return false;}

	delete m_newValue;
	m_newValue = command->m_newValue;
	auto command2 = const_cast<ColorMapSettingModifyCommand*> (command);
	command2->m_newValue = nullptr;
	return true;
}
