#ifndef POST2DWINDOWPARTICLESBASETOPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWPARTICLESBASETOPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowparticlesbasetopdataitem.h"

#include <QUndoCommand>

class Post2dWindowParticlesBaseTopDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const PostParticleBasicSetting& s, Post2dWindowParticlesBaseTopDataItem* item);

	void redo() override;
	void undo() override;

private:
	PostParticleBasicSetting m_newSetting;
	PostParticleBasicSetting m_oldSetting;

	Post2dWindowParticlesBaseTopDataItem* m_item;
};

#endif // POST2DWINDOWPARTICLESBASETOPDATAITEM_SETSETTINGCOMMAND_H
