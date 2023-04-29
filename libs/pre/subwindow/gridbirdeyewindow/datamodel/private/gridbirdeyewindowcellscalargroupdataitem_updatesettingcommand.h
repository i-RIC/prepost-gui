#ifndef GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../gridbirdeyewindowcellscalargroupdataitem.h"

class GridBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand : public QUndoCommand
{
public:
	UpdateSettingCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand);
	~UpdateSettingCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colorMapCommand;

	GridBirdEyeWindowCellScalarGroupDataItem* m_item;
};

#endif // GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
