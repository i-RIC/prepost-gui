#ifndef GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../gridbirdeyewindownodescalargroupdataitem.h"

class GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand : public QUndoCommand
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

	GridBirdEyeWindowNodeScalarGroupDataItem* m_item;
};

#endif // GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
