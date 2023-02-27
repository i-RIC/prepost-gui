#ifndef ARROWSSETTINGEDITWIDGET_MODIFYCOMMAND_H
#define ARROWSSETTINGEDITWIDGET_MODIFYCOMMAND_H

#include "../arrowssettingeditwidget.h"

#include <QUndoCommand>

class ArrowsSettingEditWidget::ModifyCommand : public QUndoCommand
{
public:
	ModifyCommand(QUndoCommand* as, QUndoCommand* cs);
	~ModifyCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:

	QUndoCommand* m_arrowsSettingCommand;
	QUndoCommand* m_colormapSettingCommand;
};

#endif // ARROWSSETTINGEDITWIDGET_MODIFYCOMMAND_H
