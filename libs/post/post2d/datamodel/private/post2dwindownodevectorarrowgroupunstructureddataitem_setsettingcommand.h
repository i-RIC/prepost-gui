#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "../post2dwindownodevectorarrowunstructuredsetting.h"
#include <QUndoCommand>

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorArrowUnstructuredSetting& setting, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item);

	void redo() override;
	void undo() override;

private:
	Post2dWindowNodeVectorArrowUnstructuredSetting m_newSetting;
	Post2dWindowNodeVectorArrowUnstructuredSetting m_oldSetting;

	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* m_item;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
