#ifndef POST3DWINDOWPARTICLESBASETOPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWPARTICLESBASETOPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowparticlesbasetopdataitem.h"

#include <QUndoCommand>

class Post3dWindowParticlesBaseTopDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const PostParticleBasicSetting& s, Post3dWindowParticlesBaseTopDataItem* item);

	void redo() override;
	void undo() override;

private:
	PostParticleBasicSetting m_newSetting;
	PostParticleBasicSetting m_oldSetting;

	Post3dWindowParticlesBaseTopDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESBASETOPDATAITEM_SETSETTINGCOMMAND_H
