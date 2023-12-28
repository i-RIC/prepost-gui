#ifndef COLORMAPLEGENDSETTINGMODIFYCOMMAND_H
#define COLORMAPLEGENDSETTINGMODIFYCOMMAND_H

#include "../guicore_global.h"

#include <QUndoCommand>

class ColorMapLegendSettingContainerI;

class ColorMapLegendSettingModifyCommand : public QUndoCommand
{
public:
	ColorMapLegendSettingModifyCommand(ColorMapLegendSettingContainerI* newValue, ColorMapLegendSettingContainerI* target);
	~ColorMapLegendSettingModifyCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	ColorMapLegendSettingContainerI* m_newValue;
	ColorMapLegendSettingContainerI* m_oldValue;

	ColorMapLegendSettingContainerI* m_target;
};

#endif // COLORMAPLEGENDSETTINGMODIFYCOMMAND_H
