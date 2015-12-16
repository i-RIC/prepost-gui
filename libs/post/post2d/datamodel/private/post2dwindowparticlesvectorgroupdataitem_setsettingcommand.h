#ifndef POST2DWINDOWPARTICLESVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWPARTICLESVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowparticlesvectorgroupdataitem.h"

#include <QUndoCommand>

class Post2dWindowParticlesVectorGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ArrowSettingContainer& s, Post2dWindowParticlesVectorGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	ArrowSettingContainer m_newSetting;
	ArrowSettingContainer m_oldSetting;

	Post2dWindowParticlesVectorGroupDataItem* m_item;
};

#endif // POST2DWINDOWPARTICLESVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
