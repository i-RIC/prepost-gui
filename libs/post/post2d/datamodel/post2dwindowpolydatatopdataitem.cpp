#include "post2dwindowpolydatagroupdataitem.h"
#include "post2dwindowpolydatatopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Post2dWindowPolyDataTopDataItem::Post2dWindowPolyDataTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Polygon"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	const auto& map = cont->polyDataMap();

	for (auto pair : map) {
		auto gItem = new Post2dWindowPolyDataGroupDataItem(pair.first, this);
		m_childItems.push_back(gItem);
	}
}

Post2dWindowPolyDataTopDataItem::~Post2dWindowPolyDataTopDataItem()
{}

void Post2dWindowPolyDataTopDataItem::update()
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post2dWindowPolyDataGroupDataItem*>(c);
		gItem->update();
	}
}

void Post2dWindowPolyDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post2dWindowPolyDataGroupDataItem*>(c);
		QDomNode cNode = iRIC::getChildNodeWithAttribute(node, "PolyDataGroup", "name", gItem->name().c_str());
		if (! cNode.isNull()) {
			gItem->loadFromProjectMainFile(cNode);
		}
	}
}

void Post2dWindowPolyDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post2dWindowPolyDataGroupDataItem*>(c);
		writer.writeStartElement("PolyDataGroup");
		writer.writeAttribute("name", gItem->name().c_str());
		gItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
