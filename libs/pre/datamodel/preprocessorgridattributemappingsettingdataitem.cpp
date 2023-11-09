#include "preprocessorgridattributemappingsettingdataitem.h"
#include "preprocessorgeodatagroupdataitem.h"

#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

PreProcessorGridAttributeMappingSettingDataItem::PreProcessorGridAttributeMappingSettingDataItem(SolverDefinitionGridAttribute* cond, PreProcessorGeoDataGroupDataItem* geodataGroup, PreProcessorDataItem* parent) :
	PreProcessorDataItem {parent},
	m_mappingMode {"mode", mmFromGeoData},
	m_condition {cond},
	m_geodataGroupDataItem {geodataGroup}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

void PreProcessorGridAttributeMappingSettingDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_mappingMode.load(node);
}

void PreProcessorGridAttributeMappingSettingDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_condition->name().c_str());
	m_mappingMode.save(writer);
}

void PreProcessorGridAttributeMappingSettingDataItem::setDefaultValue(v4InputGrid* grid)
{
	// delegate to geodatagroupdataitem.
	m_geodataGroupDataItem->setDefaultValue(grid);
}

void PreProcessorGridAttributeMappingSettingDataItem::executeMapping(v4InputGrid* grid, WaitDialog* dialog)
{
	if (m_mappingMode == mmFromGeoData) {
		// delegate to geodatagroupdataitem.
		m_geodataGroupDataItem->executeMapping(grid, dialog);
	} else {
		// @todo not implemented yet.
		// maybe we will implement this AFTER 2.0.
	}
}

int PreProcessorGridAttributeMappingSettingDataItem::mappingCount() const
{
	if (m_mappingMode == mmFromGeoData) {
		return m_geodataGroupDataItem->mappingCount();
	} else {
		// @todo not implemented yet.
	}
	return 0;
}
