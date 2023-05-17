#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../post2dbirdeyewindownodescalargroupdataitem.h"

class Post2dBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand : public QUndoCommand
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

	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_UPDATESETTINGCOMMAND_H
