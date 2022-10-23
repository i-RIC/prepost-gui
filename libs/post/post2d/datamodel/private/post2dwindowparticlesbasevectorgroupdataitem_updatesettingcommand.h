#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H

#include "../post2dwindowparticlesbasevectorgroupdataitem.h"

#include <memory>

class QUndoCommand;

class Post2dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand : public QUndoCommand
{
public:
	UpdateSettingCommand(bool apply, QUndoCommand* arrowsCommand, QUndoCommand* filterCommand, Post2dWindowParticlesBaseVectorGroupDataItem* item);
	~UpdateSettingCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	std::unique_ptr<QUndoCommand> m_arrowsCommand;
	std::unique_ptr<QUndoCommand> m_filterCommand;

	Post2dWindowParticlesBaseVectorGroupDataItem* m_item;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_UPDATESETTINGCOMMAND_H
