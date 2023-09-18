#include "geodatapolydatagroup_editnameandvaluecommand.h"
#include "geodatapolydatagroup_impl.h"
#include "../geodatapolydatagrouppolydata.h"

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>

GeoDataPolyDataGroup::EditNameAndValueCommand::EditNameAndValueCommand(const std::vector<QString>& names, const std::vector<QVariant>& values, const std::vector<GeoDataPolyDataGroupPolyData*> data, GeoDataPolyDataGroup* group) :
	QUndoCommand(GeoDataPolyDataGroup::tr("Edit name and value of polydatas")),
	m_newNames (names),
	m_newValues (values),
	m_data (data),
	m_group {group}
{
	for (auto p : data){
		m_oldNames.push_back(p->name());
		m_oldValues.push_back(p->value());
	}
}

void GeoDataPolyDataGroup::EditNameAndValueCommand::redo()
{
	apply(m_newNames, m_newValues);
}

void GeoDataPolyDataGroup::EditNameAndValueCommand::undo()
{
	apply(m_oldNames, m_oldValues);
}

void GeoDataPolyDataGroup::EditNameAndValueCommand::apply(const std::vector<QString>& names, const std::vector<QVariant>& values)
{
	for (int i = 0; i < m_data.size(); ++i) {
		auto p = m_data.at(i);
		p->setName(names.at(i));
		p->setValue(values.at(i));
	}
	m_group->updateVtkObjects();
	m_group->updateSelectedDataVtkObjects();
	m_group->updateAttributeBrowser();
	auto p = dynamic_cast<PreProcessorGeoDataDataItemI*> (m_group->parent());
	p->informValueRangeChange();
	p->informDataChange();
}
