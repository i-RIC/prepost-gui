#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindowparticlesbasevectorgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ArrowSettingContainer& s, Post3dWindowParticlesBaseVectorGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	ArrowSettingContainer m_newSetting;
	ArrowSettingContainer m_oldSetting;

	Post3dWindowParticlesBaseVectorGroupDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
