#ifndef POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H
#define POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H

#include "../post3dwindowparticlesbasescalargroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand, Post3dWindowParticlesBaseScalarGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colorMapCommand;

	Post3dWindowParticlesBaseScalarGroupDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H
