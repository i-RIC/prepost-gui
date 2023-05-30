#ifndef PARTICLEDATASETTINGEDITWIDGET_MODIFYCOMMAND_H
#define PARTICLEDATASETTINGEDITWIDGET_MODIFYCOMMAND_H

#include "../particledatasettingeditwidget.h"

#include <QUndoCommand>

class ParticleDataSettingEditWidget::ModifyCommand : public QUndoCommand
{
public:
	ModifyCommand(bool allowMerge, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_allowMerge;

	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colorMapCommand;
};

#endif // PARTICLEDATASETTINGEDITWIDGET_MODIFYCOMMAND_H
