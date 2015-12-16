#ifndef POST3DWINDOWPARTICLESTOPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWPARTICLESTOPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowparticlestopdataitem.h"

#include <QUndoCommand>

class Post3dWindowParticlesTopDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const PostParticleBasicSetting& s, Post3dWindowParticlesTopDataItem* item);

	void redo() override;
	void undo() override;

private:
	PostParticleBasicSetting m_newSetting;
	PostParticleBasicSetting m_oldSetting;

	Post3dWindowParticlesTopDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESTOPDATAITEM_SETSETTINGCOMMAND_H
