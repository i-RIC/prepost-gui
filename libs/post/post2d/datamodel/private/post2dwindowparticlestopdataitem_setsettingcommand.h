#ifndef POST2DWINDOWPARTICLESTOPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWPARTICLESTOPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowparticlestopdataitem.h"

#include <QUndoCommand>

class Post2dWindowParticlesTopDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const PostParticleBasicSetting& s, Post2dWindowParticlesTopDataItem* item);

	void redo() override;
	void undo() override;

private:
	PostParticleBasicSetting m_newSetting;
	PostParticleBasicSetting m_oldSetting;

	Post2dWindowParticlesTopDataItem* m_item;
};

#endif // POST2DWINDOWPARTICLESTOPDATAITEM_SETSETTINGCOMMAND_H
