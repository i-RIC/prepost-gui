#ifndef POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H
#define POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H

#include "../post2dwindownodevectorarrowgroupdataitem.h"

#include <QUndoCommand>

#include <memory>

class Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(bool apply, QUndoCommand* command, Post2dWindowNodeVectorArrowGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;
	std::unique_ptr<QUndoCommand> m_command;
	Post2dWindowNodeVectorArrowGroupDataItem* m_item;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H