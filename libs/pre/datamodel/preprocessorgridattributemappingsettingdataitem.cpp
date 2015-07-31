#include "preprocessorgridattributemappingsettingdataitem.h"
#include "preprocessorgeodatagroupdataitem.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QDomElement>
#include <QStandardItem>
#include <QXmlStreamWriter>

PreProcessorGridAttributeMappingSettingDataItem::PreProcessorGridAttributeMappingSettingDataItem(SolverDefinitionGridAttribute* cond, PreProcessorGeoDataGroupDataItem* geodataGroup, PreProcessorDataItem* parent) :
	PreProcessorDataItem {parent},
	m_mappingMode {"mode", mmFromGeoData}
{
	m_isDeletable = false;

	m_condition = cond;
	m_geodataGroupDataItem = geodataGroup;
}

void PreProcessorGridAttributeMappingSettingDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_mappingMode.load(node);
}

void PreProcessorGridAttributeMappingSettingDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_condition->name());
	m_mappingMode.save(writer);
}

void PreProcessorGridAttributeMappingSettingDataItem::setDefaultValue(Grid* grid)
{
	// delegate to geodatagroupdataitem.
	m_geodataGroupDataItem->setDefaultValue(grid);
}

void PreProcessorGridAttributeMappingSettingDataItem::executeMapping(Grid* grid, WaitDialog* dialog)
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
