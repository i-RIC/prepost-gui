#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowpolydatagroupdataitem.h"
#include "post2dwindowpolydatatopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Post2dWindowPolyDataTopDataItem::Post2dWindowPolyDataTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Polygon"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
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

Post2dWindowCalculationResultDataItem* Post2dWindowPolyDataTopDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowPolyDataTopDataItem::zoneDataItem() const
{
	return resultDataItem()->zoneDataItem();
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
