#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowparticlesbasevectorgroupdataitem.h"

#include <QUndoCommand>

class Post2dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ArrowSettingContainer& s, Post2dWindowParticlesBaseVectorGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	ArrowSettingContainer m_newSetting;
	ArrowSettingContainer m_oldSetting;

	Post2dWindowParticlesBaseVectorGroupDataItem* m_item;
};


#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETSETTINGCOMMAND_H
