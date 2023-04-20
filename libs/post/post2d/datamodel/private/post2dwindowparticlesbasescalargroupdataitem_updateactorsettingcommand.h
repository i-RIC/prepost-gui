#ifndef POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H
#define POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H

#include "../post2dwindowparticlesbasescalargroupdataitem.h"

#include <QUndoCommand>

class Post2dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand, Post2dWindowParticlesBaseScalarGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colorMapCommand;

	Post2dWindowParticlesBaseScalarGroupDataItem* m_item;
};

#endif // POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H
