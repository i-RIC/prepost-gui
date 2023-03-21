#ifndef GEODATAPOINTGROUP_UPDATEACTORSETTINGCOMMAND_H
#define GEODATAPOINTGROUP_UPDATEACTORSETTINGCOMMAND_H

#include "../geodatapointgroup.h"

#include <QUndoCommand>

#include <memory>

class GeoDataPointGroup::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(QUndoCommand* command, GeoDataPointGroup* group);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GeoDataPointGroup* m_group;
};

#endif // GEODATAPOINTGROUP_UPDATEACTORSETTINGCOMMAND_H
