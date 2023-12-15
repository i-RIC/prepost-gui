#include "v4inputgrid_attributedataprovider.h"
#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "../../../solverdef/solverdefinitiongridcomplexattribute.h"
#include "../../../solverdef/solverdefinitiongridtype.h"

#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroption.h>

v4InputGrid::AttributeDataProvider::AttributeDataProvider(SolverDefinitionGridType* gridType) :
	m_gridType {gridType},
	m_geoDataTopDataItem {nullptr}
{}

void v4InputGrid::AttributeDataProvider::setGeoDataTopDataItem(PreProcessorGeoDataTopDataItemI* item)
{
	m_geoDataTopDataItem = item;
}

QString v4InputGrid::AttributeDataProvider::caption(const std::string& name) const
{
	return m_gridType->gridAttributeCaption(name);
}

QVariant v4InputGrid::AttributeDataProvider::attributeBrowserValue(const std::string& name, double value) const
{
	auto att = attribute(name);

	auto iAtt = dynamic_cast<GridAttributeIntegerContainer*>(att);
	auto rAtt = dynamic_cast<GridAttributeRealContainer*>(att);
	auto cAtt = dynamic_cast<GridComplexAttributeContainer*>(att);

	if (iAtt != nullptr) {
		if (iAtt->gridAttribute()->isOption()) {
			auto optCond = dynamic_cast<SolverDefinitionGridAttributeIntegerOption*>(iAtt->gridAttribute());
			return optCond->enumerations().value(static_cast<int>(value));
		} else {
			return static_cast<int> (value);
		}
	} else if (rAtt != nullptr) {
		return value;
	} else if (cAtt != nullptr) {
		auto group1 = m_geoDataTopDataItem->groupDataItem(cAtt->name());
		auto group2 = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*> (group1);
		int groupId = static_cast<int> (value) - 1;
		if (groupId < 0) {groupId = 0;}
		auto g = group2->groups().at(groupId);
		return g->caption();
	}
	return 0;
}

ColorMapSettingContainerI* v4InputGrid::AttributeDataProvider::createColorMapSetting(const std::string& name) const
{
	return attribute(name)->createColorMapSettingContainer();
}

ColorMapLegendSettingContainerI* v4InputGrid::AttributeDataProvider::createColorMapLegendSettingContainer(const std::string& name) const
{
	return attribute(name)->createColorMapLegendSettingContainer();
}

ColorMapSettingEditWidgetI* v4InputGrid::AttributeDataProvider::createColorMapSettingEditWidget(const std::string& name, QWidget* parent) const
{
	return attribute(name)->createColorMapSettingEditWidget(parent);
}

ColorMapSettingEditDialog* v4InputGrid::AttributeDataProvider::createColorMapSettingEditDialog(const std::string& name, QWidget* parent) const
{
	return attribute(name)->createColorMapSettingEditDialog(parent);
}

ColorMapSettingToolBarWidget* v4InputGrid::AttributeDataProvider::createColorMapSettingToolbarWidget(const std::string& name, QWidget* parent) const
{
	return attribute(name)->createColorMapSettingToolbarWidget(parent);
}

GridAttributeStringConverter* v4InputGrid::AttributeDataProvider::createStringConverter(const std::string& name) const
{
	return attribute(name)->stringConverter();
}

SolverDefinitionGridAttribute* v4InputGrid::AttributeDataProvider::attribute(const std::string& name) const
{
	auto att = m_gridType->gridAttribute(name);
	if (att != nullptr) {return att;}

	auto compAtt = m_gridType->gridComplexAttribute(name);
	if (compAtt != nullptr) {return compAtt;}

	return nullptr;
}
