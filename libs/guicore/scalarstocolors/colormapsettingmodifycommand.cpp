#include "colormapsettingcontaineri.h"
#include "colormapsettingmodifycommand.h"

ColorMapSettingModifyCommand::ColorMapSettingModifyCommand(ColorMapSettingContainerI* newValue, ColorMapSettingContainerI* target) :
	m_newValue {newValue},
	m_oldValue {target->copy()},
	m_target {target}
{}

void ColorMapSettingModifyCommand::redo()
{
	m_target->copy(*m_newValue.get());
}

void ColorMapSettingModifyCommand::undo()
{
	m_target->copy(*m_oldValue.get());
}
