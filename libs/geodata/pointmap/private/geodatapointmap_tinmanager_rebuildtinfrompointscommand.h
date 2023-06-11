#ifndef GEODATAPOINTMAP_TINMANAGER_REBUILDTINFROMPOINTSCOMMAND_H
#define GEODATAPOINTMAP_TINMANAGER_REBUILDTINFROMPOINTSCOMMAND_H

#include "geodatapointmap_tinmanager.h"

#include <QUndoCommand>

#include <memory>

class GeoDataPointmap::TINManager::RebuildTinFromPointsCommand : public QUndoCommand
{
public:
	RebuildTinFromPointsCommand(QUndoCommand* command, TINManager* manager);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	TINManager* m_manager;
};

#endif // GEODATAPOINTMAP_TINMANAGER_REBUILDTINFROMPOINTSCOMMAND_H
