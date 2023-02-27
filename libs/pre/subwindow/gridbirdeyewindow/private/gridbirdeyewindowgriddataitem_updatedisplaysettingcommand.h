#ifndef GRIDBIRDEYEWINDOWGRIDDATAITEM_UPDATEDISPLAYSETTINGCOMMAND_H
#define GRIDBIRDEYEWINDOWGRIDDATAITEM_UPDATEDISPLAYSETTINGCOMMAND_H

#include "../gridbirdeyewindowgriddataitem.h"

#include <QUndoCommand>

class GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand : public QUndoCommand
{
public:
	UpdateDisplaySettingCommand(bool apply, QUndoCommand* s, QUndoCommand* cs);
	~UpdateDisplaySettingCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colormapSettingCommand;
};

#endif // GRIDBIRDEYEWINDOWGRIDDATAITEM_UPDATEDISPLAYSETTINGCOMMAND_H
