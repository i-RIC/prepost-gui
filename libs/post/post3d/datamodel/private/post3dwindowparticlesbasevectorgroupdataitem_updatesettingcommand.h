#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../post3dwindowparticlesbasevectorgroupdataitem.h"

#include <memory>

class QUndoCommand;

class Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand : public QUndoCommand
{
public:
	UpdateSettingCommand(bool apply, QUndoCommand* arrowsCommand, QUndoCommand* filterCommand, Post3dWindowParticlesBaseVectorGroupDataItem* item);
	~UpdateSettingCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	std::unique_ptr<QUndoCommand> m_arrowsCommand;
	std::unique_ptr<QUndoCommand> m_filterCommand;

	Post3dWindowParticlesBaseVectorGroupDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H
