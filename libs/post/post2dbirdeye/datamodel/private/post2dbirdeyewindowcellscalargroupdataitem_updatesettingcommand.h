#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../post2dbirdeyewindowcellscalargroupdataitem.h"

class Post2dBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand : public QUndoCommand
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

	Post2dBirdEyeWindowCellScalarGroupDataItem* m_item;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
