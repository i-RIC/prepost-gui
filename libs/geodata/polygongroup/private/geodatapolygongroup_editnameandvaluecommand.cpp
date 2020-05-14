#include "geodatapolygongroup_editnameandvaluecommand.h"
#include "geodatapolygongroup_impl.h"
#include "../geodatapolygongrouppolygon.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>

GeoDataPolygonGroup::EditNameAndValueCommand::EditNameAndValueCommand(const std::vector<QString>& names, const std::vector<QVariant>& values, const std::vector<GeoDataPolygonGroupPolygon*> polygons, GeoDataPolygonGroup* group) :
	QUndoCommand(GeoDataPolygonGroup::tr("Edit name and value of polygons")),
	m_newNames (names),
	m_newValues (values),
	m_polygons (polygons),
	m_group {group}
{
	for (auto p : polygons){
		m_oldNames.push_back(p->name());
		m_oldValues.push_back(p->value());
	}
}

void GeoDataPolygonGroup::EditNameAndValueCommand::redo()
{
	apply(m_newNames, m_newValues);
}

void GeoDataPolygonGroup::EditNameAndValueCommand::undo()
{
	apply(m_oldNames, m_oldValues);
}

void GeoDataPolygonGroup::EditNameAndValueCommand::apply(const std::vector<QString>& names, const std::vector<QVariant>& values)
{
	for (int i = 0; i < m_polygons.size(); ++i) {
		auto p = m_polygons.at(i);
		p->setName(names.at(i));
		p->setValue(values.at(i));
	}
	m_group->updateVtkObjects();
	m_group->impl->updateSelectedPolygonsVtkObjects();
	m_group->updateAttributeBrowser();
	auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (m_group->parent());
	p->informValueRangeChange();
	p->informDataChange();
}
