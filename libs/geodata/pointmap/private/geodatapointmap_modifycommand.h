#ifndef GEODATAPOINTMAP_MODIFYCOMMAND_H
#define GEODATAPOINTMAP_MODIFYCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

#include <memory>

class GeoDataPointmap::ModifyCommand : public QUndoCommand
{
public:
	ModifyCommand(QUndoCommand* command, GeoDataPointmap* pointmap);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_oldMapped;
	bool m_oldNeedRebuildTin;

	std::unique_ptr<QUndoCommand> m_command;
	GeoDataPointmap* m_pointmap;
};

#endif // GEODATAPOINTMAP_MODIFYCOMMAND_H
