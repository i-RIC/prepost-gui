#include "preprocessorgridattributemappingsettingdataitem.h"
#include "preprocessorgeodatagroupdataitem.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QDomElement>
#include <QStandardItem>
#include <QXmlStreamWriter>

PreProcessorGridAttributeMappingSettingDataItem::PreProcessorGridAttributeMappingSettingDataItem(SolverDefinitionGridAttribute* cond, PreProcessorGeoDataGroupDataItem* geodataGroup, PreProcessorDataItem* parent)
	: PreProcessorDataItem(parent)
{
	m_mappingMode = mmFromGeoData;
	m_isDeletable = false;

	m_condition = cond;
	m_geodataGroupDataItem = geodataGroup;

	// this node does not have corresponding standard item.
//	m_standardItem->parent()->removeRow(m_standardItem->row());
//	m_standardItem = nullptr;
}

void PreProcessorGridAttributeMappingSettingDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	if (elem.attribute("mode") == "fromOtherAttribute") {
		m_mappingMode = mmFromOtherAttribute;
	} else {
		m_mappingMode = mmFromGeoData;
	}
}

void PreProcessorGridAttributeMappingSettingDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString modestr;
	if (m_mappingMode == mmFromGeoData) {
		modestr = "fromGeoData";
	} else {
		modestr = "fromOtherAttribute";
	}
	writer.writeAttribute("name", m_condition->name());
	writer.writeAttribute("mode", modestr);
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
