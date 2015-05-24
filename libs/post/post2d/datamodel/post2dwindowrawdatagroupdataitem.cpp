#include "post2dwindowrawdatadataitem.h"
#include "post2dwindowrawdatagroupdataitem.h"
#include "post2dwindowrawdatatopdataitem.h"

#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include <guicore/pre/rawdata/rawdata.h>
#include <guicore/pre/rawdata/rawdataproxy.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>

#include <QMap>

Post2dWindowRawDataGroupDataItem::Post2dWindowRawDataGroupDataItem(SolverDefinitionGridRelatedCondition* cond, Post2dWindowDataItem* parent)
	: Post2dWindowDataItem(cond->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_condition = cond;
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
}


void Post2dWindowRawDataGroupDataItem::updateChildren()
{
	Post2dWindowRawDataTopDataItem* tItem = dynamic_cast<Post2dWindowRawDataTopDataItem*>(parent());
	PreProcessorRawDataTopDataItemInterface* rtItem = tItem->preRawDataTopDataItem();
	PreProcessorRawDataGroupDataItemInterface* gItem = rtItem->groupDataItem(m_condition->name());

	QList <GraphicsWindowDataItem*> oldChildren = m_childItems;
	QMap<RawData*, Post2dWindowRawDataDataItem*> map;
	for (int i = 0; i < oldChildren.count(); ++i) {
		Post2dWindowRawDataDataItem* item = dynamic_cast<Post2dWindowRawDataDataItem*>(oldChildren.at(i));
		map.insert(item->rawDataProxy()->rawData(), item);
		m_standardItem->takeRow(0);
	}
	m_childItems.clear();
	m_itemNameMap.clear();

	QList <GraphicsWindowDataItem*> origChildren = gItem->childItems();
	for (int i = 0; i < origChildren.count(); ++i) {
		PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(origChildren.at(i));
		RawData* rawData = item->rawData();
		if (map.contains(rawData)) {
			m_childItems.append(map.value(rawData));
			m_standardItem->appendRow(map.value(rawData)->standardItem());
			oldChildren.removeOne(map.value(rawData));
			m_itemNameMap.insert(rawData->name(), map.value(rawData));
		} else {
			// try to add.
			RawDataProxy* proxy = rawData->getProxy();
			connect(rawData, SIGNAL(graphicsUpdated()), proxy, SLOT(updateGraphics()));
			if (proxy != nullptr) {
				Post2dWindowRawDataDataItem* pItem = new Post2dWindowRawDataDataItem(this);
				pItem->setRawDataProxy(proxy);
				proxy->setupDataItem();
				m_childItems.append(pItem);
				m_itemNameMap.insert(rawData->name(), pItem);
				connect(item, SIGNAL(captionChanged(QString)), pItem, SLOT(updateCaption(QString)));
			}
		}
	}
	for (int i = 0; i < oldChildren.count(); ++i) {
		delete oldChildren.at(i);
	}
	assignActorZValues(m_zDepthRange);
	updateItemMap();
}

void Post2dWindowRawDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement elem = children.at(i).toElement();
		QString name = elem.attribute("name");
		if (m_itemNameMap.contains(name)) {
			Post2dWindowRawDataDataItem* item = m_itemNameMap.value(name);
			item->loadFromProjectMainFile(elem);
		}
	}
	updateChildren();
}

void Post2dWindowRawDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_condition->name());
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("RawData");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
