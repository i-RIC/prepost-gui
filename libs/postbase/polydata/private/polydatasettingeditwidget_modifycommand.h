#ifndef POLYDATASETTINGEDITWIDGET_MODIFYCOMMAND_H
#define POLYDATASETTINGEDITWIDGET_MODIFYCOMMAND_H

#include "../polydatasettingeditwidget.h"

class PolyDataSettingEditWidget::ModifyCommand : public QUndoCommand
{
public:
	ModifyCommand(bool allowMerge, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand);
	~ModifyCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_allowMerge;

	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colorMapCommand;
};

#endif // POLYDATASETTINGEDITWIDGET_MODIFYCOMMAND_H
