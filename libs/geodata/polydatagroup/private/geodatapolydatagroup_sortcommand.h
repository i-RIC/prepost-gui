#ifndef GEODATAPOLYDATAGROUP_SORTCOMMAND_H
#define GEODATAPOLYDATAGROUP_SORTCOMMAND_H

#include "../geodatapolydatagroup.h"

#include <QUndoCommand>

class GeoDataPolyDataGroup::SortCommand : public QUndoCommand
{
public:
	SortCommand(const std::vector<GeoDataPolyDataGroupPolyData*> newData, GeoDataPolyDataGroup* group);

	void redo() override;
	void undo() override;

private:
	std::vector<GeoDataPolyDataGroupPolyData*> m_newData;
	std::vector<GeoDataPolyDataGroupPolyData*> m_oldData;

	GeoDataPolyDataGroup* m_group;
};

#endif // GEODATAPOLYDATAGROUP_SORTCOMMAND_H
