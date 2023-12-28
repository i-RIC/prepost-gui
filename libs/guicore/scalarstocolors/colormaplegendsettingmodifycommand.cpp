#include "colormaplegendsettingcontaineri.h"
#include "colormaplegendsettingmodifycommand.h"

#include <misc/qundocommandhelper.h>

ColorMapLegendSettingModifyCommand::ColorMapLegendSettingModifyCommand(ColorMapLegendSettingContainerI* newValue, ColorMapLegendSettingContainerI* target) :
	m_newValue {newValue},
	m_oldValue {target->copy()},
	m_target {target}
{}

ColorMapLegendSettingModifyCommand::~ColorMapLegendSettingModifyCommand()
{
	delete m_newValue;
	delete m_oldValue;
}

void ColorMapLegendSettingModifyCommand::redo()
{
	m_target->copy(*m_newValue);
}

void ColorMapLegendSettingModifyCommand::undo()
{
	m_target->copy(*m_oldValue);
}

int ColorMapLegendSettingModifyCommand::id() const
{
	return iRIC::generateCommandId("ColorMapLegendSettingModifyCommand");
}

bool ColorMapLegendSettingModifyCommand::mergeWith(const QUndoCommand* other)
{
	auto command = dynamic_cast<const ColorMapLegendSettingModifyCommand*> (other);
	if (command == nullptr) {return false;}

	if (m_target != command->m_target) {return false;}

	delete m_newValue;
	m_newValue = command->m_newValue;
	auto command2 = const_cast<ColorMapLegendSettingModifyCommand*> (command);
	command2->m_newValue = nullptr;
	return true;
}
