#ifndef COLORMAPSETTINGMODIFYCOMMAND_H
#define COLORMAPSETTINGMODIFYCOMMAND_H

#include "../guicore_global.h"

#include <QUndoCommand>

class ColorMapSettingContainerI;

class GUICOREDLL_EXPORT ColorMapSettingModifyCommand : public QUndoCommand
{
public:
	ColorMapSettingModifyCommand(ColorMapSettingContainerI* newValue, ColorMapSettingContainerI* target);
	~ColorMapSettingModifyCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	ColorMapSettingContainerI* m_newValue;
	ColorMapSettingContainerI* m_oldValue;

	ColorMapSettingContainerI* m_target;
};

#endif // COLORMAPSETTINGMODIFYCOMMAND_H
