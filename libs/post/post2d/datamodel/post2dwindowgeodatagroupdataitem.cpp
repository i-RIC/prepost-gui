#include "post2dwindowgeodatadataitem.h"
#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodataproxy.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <QDomNodeList>
#include <QMap>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <algorithm>

Post2dWindowGeoDataGroupDataItem::Post2dWindowGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {cond->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	m_condition {cond}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

void Post2dWindowGeoDataGroupDataItem::updateChildren()
{
	Post2dWindowGeoDataTopDataItem* tItem = dynamic_cast<Post2dWindowGeoDataTopDataItem*>(parent());
	PreProcessorGeoDataTopDataItemInterface* rtItem = tItem->preGeoDataTopDataItem();
	PreProcessorGeoDataGroupDataItemInterface* gItem = rtItem->groupDataItem(m_condition->name());

	std::vector<GraphicsWindowDataItem*> oldChildren = m_childItems;
	QMap<GeoData*, Post2dWindowGeoDataDataItem*> map;
	for (int i = 0; i < oldChildren.size(); ++i) {
		Post2dWindowGeoDataDataItem* item = dynamic_cast<Post2dWindowGeoDataDataItem*>(oldChildren.at(i));
		map.insert(item->geoDataProxy()->geoData(), item);
		m_standardItem->takeRow(0);
	}
	m_childItems.clear();
	m_itemNameMap.clear();

	std::vector<GraphicsWindowDataItem*> origChildren = gItem->childItems();
	for (int i = 0; i < origChildren.size(); ++i) {
		PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(origChildren.at(i));
		GeoData* geoData = item->geoData();
		if (map.contains(geoData)) {
			m_childItems.push_back(map.value(geoData));
			m_standardItem->appendRow(map.value(geoData)->standardItem());
			auto it = std::find(oldChildren.begin(), oldChildren.end(), map.value(geoData));
			oldChildren.erase(it);
			m_itemNameMap.insert(geoData->name(), map.value(geoData));
		} else {
			// try to add.
			GeoDataProxy* proxy = geoData->getProxy();
			connect(geoData, SIGNAL(graphicsUpdated()), proxy, SLOT(updateGraphics()));
			if (proxy != nullptr) {
				Post2dWindowGeoDataDataItem* pItem = new Post2dWindowGeoDataDataItem(this);
				pItem->setGeoDataProxy(proxy);
				proxy->setupDataItem();
				m_childItems.push_back(pItem);
				m_itemNameMap.insert(geoData->name(), pItem);
				connect(item, SIGNAL(captionChanged(QString)), pItem, SLOT(updateCaption(QString)));
			}
		}
	}
	for (int i = 0; i < oldChildren.size(); ++i) {
		delete oldChildren.at(i);
	}
	assignActorZValues(m_zDepthRange);
	updateItemMap();
}

void Post2dWindowGeoDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement elem = children.at(i).toElement();
		QString name = elem.attribute("name");
		if (m_itemNameMap.contains(name)) {
			Post2dWindowGeoDataDataItem* item = m_itemNameMap.value(name);
			item->loadFromProjectMainFile(elem);
		}
	}
	updateChildren();
}

void Post2dWindowGeoDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_condition->name().c_str());
	for (auto child : m_childItems) {
		writer.writeStartElement("GeoData");
		child->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
