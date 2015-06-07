#include "graph2dhybridwindowresultcopydataitem.h"
#include "graph2dhybridwindowresultcopygroupdataitem.h"
#include "graph2dhybridwindowresultdataitem.h"

#include <QStandardItem>

Graph2dHybridWindowResultCopyGroupDataItem::Graph2dHybridWindowResultCopyGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dHybridWindowDataItem(tr("Calcuation Results Copys"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;
}

Graph2dHybridWindowResultCopyGroupDataItem::~Graph2dHybridWindowResultCopyGroupDataItem()
{

}

void Graph2dHybridWindowResultCopyGroupDataItem::updateData()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Graph2dHybridWindowResultCopyDataItem* item = dynamic_cast<Graph2dHybridWindowResultCopyDataItem*>(*it);
		item->update();
	}
}

void Graph2dHybridWindowResultCopyGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList list = node.childNodes();
	for (int i = 0; i < list.count(); ++i) {
		QDomNode child = list.at(i);
		Graph2dHybridWindowResultCopyDataItem* citem = new Graph2dHybridWindowResultCopyDataItem(this);
		citem->loadFromProjectMainFile(child);
		m_childItems.append(citem);
	}
	updateData();
}

void Graph2dHybridWindowResultCopyGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dHybridWindowResultCopyDataItem* citem = dynamic_cast<Graph2dHybridWindowResultCopyDataItem*>(m_childItems.at(i));
		writer.writeStartElement("Copy");
		citem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Graph2dHybridWindowResultCopyGroupDataItem::addItem(Graph2dHybridWindowResultCopyDataItem* item)
{
	m_childItems.append(item);
}

void Graph2dHybridWindowResultCopyGroupDataItem::clear()
{
	for (int i = m_childItems.count() - 1; i >= 0; --i) {
		delete m_childItems.at(i);
	}
}

bool Graph2dHybridWindowResultCopyGroupDataItem::axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const
{
	bool needed = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dHybridWindowResultCopyDataItem* rItem = dynamic_cast<Graph2dHybridWindowResultCopyDataItem*>(m_childItems.at(i));
		needed = needed || rItem->axisNeeded(as);
	}
	return needed;
}
