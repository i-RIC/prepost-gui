#ifndef GEODATAPOLYGONGROUP_SORTCOMMAND_H
#define GEODATAPOLYGONGROUP_SORTCOMMAND_H

#include "../geodatapolygongroup.h"

#include <QUndoCommand>

class GeoDataPolygonGroup::SortCommand : public QUndoCommand
{
public:
	SortCommand(const std::vector<GeoDataPolygonGroupPolygon*> newPols, GeoDataPolygonGroup* group);

	void redo() override;
	void undo() override;

private:
	std::vector<GeoDataPolygonGroupPolygon*> m_newPolygons;
	std::vector<GeoDataPolygonGroupPolygon*> m_oldPolygons;

	GeoDataPolygonGroup* m_group;
};

#endif // GEODATAPOLYGONGROUP_SORTCOMMAND_H
