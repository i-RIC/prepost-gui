#include "preprocessorhydraulicdatadataitem.h"

#include <guicore/pre/hydraulicdata/hydraulicdata.h>

PreProcessorHydraulicDataDataItem::PreProcessorHydraulicDataDataItem(PreProcessorDataItem* parent) :
	PreProcessorHydraulicDataDataItemInterface (parent)
{}

HydraulicData* PreProcessorHydraulicDataDataItem::hydraulicData() const
{
	return m_hydraulicData;
}

void PreProcessorHydraulicDataDataItem::setHydraulicData(HydraulicData* data)
{
	m_hydraulicData = data;
}

QStringList PreProcessorHydraulicDataDataItem::containedFiles()
{
	return m_hydraulicData->containedFiles();
}

void PreProcessorHydraulicDataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_hydraulicData->loadFromProjectMainFile(node);
}

void PreProcessorHydraulicDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_hydraulicData->saveToProjectMainFile(writer);
}
