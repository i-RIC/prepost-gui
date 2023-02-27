#ifndef POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post3dwindownodevectorarrowgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* updateArrowsCommand, QUndoCommand* updateSettingsCommand, QUndoCommand* updateColormapsCommand, Post3dWindowNodeVectorArrowGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_updateArrowsCommand;
	QUndoCommand* m_updateSettingsCommand;
	QUndoCommand* m_updateColormapsCommand;
	Post3dWindowNodeVectorArrowGroupDataItem* m_item;
};

#endif // POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
