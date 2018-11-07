#include "preprocessorhydraulicdatagroupdataitem.h"
#include "preprocessorhydraulicdatatopdataitem.h"
#include "../factory/hydraulicdatafactory.h"

#include <guicore/pre/hydraulicdata/hydraulicdatacreator.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorhydraulicdatagroupdataiteminterface.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QXmlStreamWriter>

PreProcessorHydraulicDataTopDataItem::PreProcessorHydraulicDataTopDataItem(PreProcessorDataItem* parent) :
	PreProcessorHydraulicDataTopDataItemInterface (parent)
{
	setSubPath("hydraulicdata");
	auto& factory = HydraulicDataFactory::instance();
	for (auto creator : factory.creators()) {
		auto group = new PreProcessorHydraulicDataGroupDataItem(creator, this);
		m_childItems.push_back(group);
		m_itemNameMap.insert({creator->typeName(), group});
	}
}

QList<PreProcessorHydraulicDataGroupDataItemInterface*> PreProcessorHydraulicDataTopDataItem::groupDataItems() const
{
	QList<PreProcessorHydraulicDataGroupDataItemInterface*> ret;
	for (auto child : m_childItems) {
		ret.push_back(dynamic_cast<PreProcessorHydraulicDataGroupDataItemInterface*> (child));
	}
	return ret;
}

PreProcessorHydraulicDataGroupDataItemInterface* PreProcessorHydraulicDataTopDataItem::groupDataItem(const std::string& name) const
{
	auto it = m_itemNameMap.find(name);
	if (it == m_itemNameMap.end()) {return nullptr;}

	return it->second;
}

void PreProcessorHydraulicDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		auto elem = children.at(i).toElement();
		std::string name = iRIC::toStr(elem.attribute("name"));

		auto it = m_itemNameMap.find(name);
		if (it == m_itemNameMap.end()) {continue;}

		it->second->loadFromProjectMainFile(elem);
	}
}

void PreProcessorHydraulicDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto child : m_childItems) {
		writer.writeStartElement("HydraulicDataGroup");
		child->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
