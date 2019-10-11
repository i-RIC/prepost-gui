#if SKIP
#include "graph2dverificationwindowresultcopydataitem.h"
#include "graph2dverificationwindowresultcopygroupdataitem.h"
#include "graph2dverificationwindowresultdataitem.h"

#include <QDomNode>
#include <QStandardItem>

Graph2dVerificationWindowResultCopyGroupDataItem::Graph2dVerificationWindowResultCopyGroupDataItem(Graph2dWindowDataItem* parent)
	: Graph2dVerificationWindowDataItem(tr("Calcuation Results Copys"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_isDeletable = false;
}

Graph2dVerificationWindowResultCopyGroupDataItem::~Graph2dVerificationWindowResultCopyGroupDataItem()
{

}

void Graph2dVerificationWindowResultCopyGroupDataItem::updateData()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Graph2dVerificationWindowResultCopyDataItem* item = dynamic_cast<Graph2dVerificationWindowResultCopyDataItem*>(*it);
		item->update();
	}
}

void Graph2dVerificationWindowResultCopyGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList list = node.childNodes();
	for (int i = 0; i < list.count(); ++i) {
		QDomNode child = list.at(i);
		Graph2dVerificationWindowResultCopyDataItem* citem = new Graph2dVerificationWindowResultCopyDataItem(this);
		citem->loadFromProjectMainFile(child);
		m_childItems.append(citem);
	}
	updateData();
}

void Graph2dVerificationWindowResultCopyGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dVerificationWindowResultCopyDataItem* citem = dynamic_cast<Graph2dVerificationWindowResultCopyDataItem*>(m_childItems.at(i));
		writer.writeStartElement("Copy");
		citem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Graph2dVerificationWindowResultCopyGroupDataItem::addItem(Graph2dVerificationWindowResultCopyDataItem* item)
{
	m_childItems.append(item);
}

void Graph2dVerificationWindowResultCopyGroupDataItem::clear()
{
	for (int i = m_childItems.count() - 1; i >= 0; --i) {
		delete m_childItems.at(i);
	}
}

bool Graph2dVerificationWindowResultCopyGroupDataItem::axisNeeded(Graph2dVerificationWindowResultSetting::AxisSide as) const
{
	bool needed = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		Graph2dVerificationWindowResultCopyDataItem* rItem = dynamic_cast<Graph2dVerificationWindowResultCopyDataItem*>(m_childItems.at(i));
		needed = needed || rItem->axisNeeded(as);
	}
	return needed;
}
#endif
