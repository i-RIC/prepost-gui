#include "measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatavectorgroupdataitem.h"
#include "measureddatavectorgrouptopdataitem.h"

#include <guicore/project/measured/measureddata.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QStandardItem>
#include <QXmlStreamWriter>

MeasuredDataVectorGroupTopDataItem::MeasuredDataVectorGroupTopDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent)->measuredData();

	for (const auto& name : md->vectorNames()) {
		auto item = new MeasuredDataVectorGroupDataItem(name, this);
		m_childItems.push_back(item);
	}
}

MeasuredDataVectorGroupTopDataItem::~MeasuredDataVectorGroupTopDataItem()
{
	clearChildItems();
}

void MeasuredDataVectorGroupTopDataItem::updateZDepthRangeItemCount()
{
	return m_zDepthRange.setItemCount(m_childItems.size());
}

void MeasuredDataVectorGroupTopDataItem::update()
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<MeasuredDataVectorGroupDataItem*> (child);
		item->update();
	}
}

MeasuredDataFileDataItem* MeasuredDataVectorGroupTopDataItem::fileDataItem() const
{
	return dynamic_cast<MeasuredDataFileDataItem*> (parent());
}

std::unordered_set<ColorMapSettingContainerI*> MeasuredDataVectorGroupTopDataItem::activeColorMapSettingsWithVisibleLegend() const
{
	std::unordered_set<ColorMapSettingContainerI*> ret;

	for (auto c : m_childItems) {
		auto item = dynamic_cast<MeasuredDataVectorGroupDataItem*> (c);
		auto cm = item->activeColorMapSettingWithVisibleLegend();
		if (cm != nullptr) {ret.insert(cm);}
	}

	return ret;
}

void MeasuredDataVectorGroupTopDataItem::showPropertyDialog()
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<MeasuredDataVectorGroupDataItem*> (child);
		if (item->standardItem()->checkState() != Qt::Checked) {continue;}

		// open property dialog for the first checked item
		item->showPropertyDialog();
		return;
	}

	for (auto child : m_childItems) {
		auto item = dynamic_cast<MeasuredDataVectorGroupDataItem*> (child);

		// if all items are unchecked, open property dialog for the first item.
		item->showPropertyDialog();
		return;
	}
}

void MeasuredDataVectorGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	std::unordered_map<std::string, MeasuredDataVectorGroupDataItem*> itemMap;
	for (auto c : m_childItems) {
		auto item = dynamic_cast<MeasuredDataVectorGroupDataItem*> (c);
		itemMap.insert({item->target(), item});
	}

	if (node.nodeName() == "VectorData") {
		// old style
		auto target = iRIC::toStr(node.toElement().attribute("attribute"));
		auto it = itemMap.find(target);
		if (it != itemMap.end()) {
			it->second->loadFromProjectMainFile(node);
		}
	} else if (node.nodeName() == "VectorTopData") {
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "VectorData") {
				auto target = iRIC::toStr(childElem.attribute("target"));
				auto it = itemMap.find(target);
				if (it != itemMap.end()) {
					it->second->loadFromProjectMainFile(childElem);
				}
			}
		}
	}
}

void MeasuredDataVectorGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto child : m_childItems) {
		writer.writeStartElement("VectorData");
		auto item = dynamic_cast<MeasuredDataVectorGroupDataItem*> (child);
		writer.writeAttribute("target", item->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
