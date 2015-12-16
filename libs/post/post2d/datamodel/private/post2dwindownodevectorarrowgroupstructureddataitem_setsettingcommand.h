#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindownodevectorarrowgroupstructureddataitem.h"

#include <QUndoCommand>

class Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorArrowStructuredSetting& setting, Post2dWindowNodeVectorArrowGroupStructuredDataItem* item);

	void redo() override;
	void undo() override;

	Post2dWindowNodeVectorArrowStructuredSetting m_newSetting;
	Post2dWindowNodeVectorArrowStructuredSetting m_oldSetting;

	Post2dWindowNodeVectorArrowGroupStructuredDataItem* m_item;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
