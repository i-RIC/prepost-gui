#ifndef COLORMAPSETTINGMODIFYCOMMAND_H
#define COLORMAPSETTINGMODIFYCOMMAND_H

#include "../guicore_global.h"

#include <QUndoCommand>

#include <memory>

class ColorMapSettingContainerI;

class GUICOREDLL_EXPORT ColorMapSettingModifyCommand : public QUndoCommand
{
public:
	ColorMapSettingModifyCommand(ColorMapSettingContainerI* newValue, ColorMapSettingContainerI* target);

	void redo() override;
	void undo() override;

private:
	std::unique_ptr<ColorMapSettingContainerI> m_newValue;
	std::unique_ptr<ColorMapSettingContainerI> m_oldValue;

	ColorMapSettingContainerI* m_target;
};

#endif // COLORMAPSETTINGMODIFYCOMMAND_H
