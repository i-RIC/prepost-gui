#ifndef GEODATAPOLYGONGROUP_SORTEDITTARGETPOLYGONCOMMAND_H
#define GEODATAPOLYGONGROUP_SORTEDITTARGETPOLYGONCOMMAND_H

#include "../geodatapolygongroup.h"

#include <QUndoCommand>

class GeoDataPolygonGroup::SortEditTargetPolygonCommand : public QUndoCommand
{
public:
	SortEditTargetPolygonCommand(int newIndex, GeoDataPolygonGroup* group);

	void redo() override;
	void undo() override;

private:
	int m_newIndex;
	int m_oldIndex;

	GeoDataPolygonGroup* m_group;
};

#endif // GEODATAPOLYGONGROUP_SORTEDITTARGETPOLYGONCOMMAND_H
