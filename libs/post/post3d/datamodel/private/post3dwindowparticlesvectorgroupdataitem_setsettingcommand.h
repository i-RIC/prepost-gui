#ifndef POST3DWINDOWPARTICLESVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWPARTICLESVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowparticlesvectorgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowParticlesVectorGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ArrowSettingContainer& s, Post3dWindowParticlesVectorGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	ArrowSettingContainer m_newSetting;
	ArrowSettingContainer m_oldSetting;

	Post3dWindowParticlesVectorGroupDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
