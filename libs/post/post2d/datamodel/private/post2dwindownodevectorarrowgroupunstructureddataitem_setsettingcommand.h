#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "../post2dwindownodevectorarrowunstructuredsetting.h"
#include <QUndoCommand>

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorArrowUnstructuredSetting& setting, const QString& scalarBarTitle, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item);

	void redo() override;
	void undo() override;

private:
	Post2dWindowNodeVectorArrowUnstructuredSetting m_newSetting;
	QString m_newScalarBarTitle;

	Post2dWindowNodeVectorArrowUnstructuredSetting m_oldSetting;
	QString m_oldScalarBarTitle;

	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* m_item;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
