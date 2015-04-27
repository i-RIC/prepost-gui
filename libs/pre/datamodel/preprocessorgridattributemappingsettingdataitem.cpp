#include "preprocessorgridattributemappingsettingdataitem.h"
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include "preprocessorrawdatagroupdataitem.h"
#include <guicore/pre/grid/grid.h>
#include <QXmlStreamWriter>
#include <QDomElement>
#include <QStandardItem>

PreProcessorGridAttributeMappingSettingDataItem::PreProcessorGridAttributeMappingSettingDataItem(SolverDefinitionGridRelatedCondition* cond, PreProcessorRawDataGroupDataItem* rawdataGroup, PreProcessorDataItem* parent)
	: PreProcessorDataItem(parent)
{
	m_mappingMode = mmFromRawData;
	m_isDeletable = false;

	m_condition = cond;
	m_rawdataGroupDataItem = rawdataGroup;

	// this node does not have corresponding standard item.
//	m_standardItem->parent()->removeRow(m_standardItem->row());
//	m_standardItem = nullptr;
}

void PreProcessorGridAttributeMappingSettingDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	if (elem.attribute("mode") == "fromOtherAttribute"){
		m_mappingMode = mmFromOtherAttribute;
	}else{
		m_mappingMode = mmFromRawData;
	}
}

void PreProcessorGridAttributeMappingSettingDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString modestr;
	if (m_mappingMode == mmFromRawData){
		modestr = "fromRawData";
	}else{
		modestr = "fromOtherAttribute";
	}
	writer.writeAttribute("name", m_condition->name());
	writer.writeAttribute("mode", modestr);
}

void PreProcessorGridAttributeMappingSettingDataItem::setDefaultValue(Grid* grid)
{
	// delegate to rawdatagroupdataitem.
	m_rawdataGroupDataItem->setDefaultValue(grid);
}

void PreProcessorGridAttributeMappingSettingDataItem::executeMapping(Grid* grid, WaitDialog *dialog)
{
	if (m_mappingMode == mmFromRawData){
		// delegate to rawdatagroupdataitem.
		m_rawdataGroupDataItem->executeMapping(grid, dialog);
	} else {
		// @todo not implemented yet.
		// maybe we will implement this AFTER 2.0.
	}
}

int PreProcessorGridAttributeMappingSettingDataItem::mappingCount() const
{
	if (m_mappingMode == mmFromRawData){
		return m_rawdataGroupDataItem->mappingCount();
	} else {
		// @todo not implemented yet.
	}
}
