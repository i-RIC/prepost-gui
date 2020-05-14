#ifndef GEODATAPOLYGONGROUP_EDITNAMEANDVALUECOMMAND_H
#define GEODATAPOLYGONGROUP_EDITNAMEANDVALUECOMMAND_H

#include "../geodatapolygongroup.h"

#include <QString>
#include <QUndoCommand>
#include <QVariant>

#include <vector>

class GeoDataPolygonGroup::EditNameAndValueCommand : public QUndoCommand
{
public:
	EditNameAndValueCommand(const std::vector<QString>& names, const std::vector<QVariant>& values, const std::vector<GeoDataPolygonGroupPolygon*> polygons, GeoDataPolygonGroup* group);

	void redo() override;
	void undo() override;

private:
	void apply(const std::vector<QString>& names, const std::vector<QVariant>& values);

	std::vector<QString> m_newNames;
	std::vector<QVariant> m_newValues;

	std::vector<QString> m_oldNames;
	std::vector<QVariant> m_oldValues;

	std::vector<GeoDataPolygonGroupPolygon*> m_polygons;
	GeoDataPolygonGroup* m_group;
};

#endif // GEODATAPOLYGONGROUP_EDITNAMEANDVALUECOMMAND_H
