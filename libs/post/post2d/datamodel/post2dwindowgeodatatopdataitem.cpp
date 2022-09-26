#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"

#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <pre/datamodel/preprocessorgeodatatopdataitem.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <algorithm>

Post2dWindowGeoDataTopDataItem::Post2dWindowGeoDataTopDataItem(PreProcessorGeoDataTopDataItemInterface* ditem, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Geographic Data"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_preGeoDataTopDataItem {ditem}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	// add child nodes.
	for (SolverDefinitionGridAttribute* att : ditem->gridType()->gridAttributes()) {
		std::string lowerName = att->name();
		std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
		if (lowerName.substr(0, 9) != "elevation") {continue;}

		auto item = new Post2dWindowGeoDataGroupDataItem(att, this);
		m_childItems.push_back(item);
		m_itemNameMap.insert(att->name(), item);
	}
	updateChildren();
	connect(ditem, SIGNAL(dataChanged()), this, SLOT(updateChildren()));
}

void Post2dWindowGeoDataTopDataItem::updateChildren()
{
	for (int i = 0; i < m_childItems.size(); ++i) {
		auto gItem = dynamic_cast<Post2dWindowGeoDataGroupDataItem*>(m_childItems.at(i));
		gItem->updateChildren();
	}
	assignActorZValues(m_zDepthRange);
}

void Post2dWindowGeoDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		std::string name = iRIC::toStr(child.attribute("name"));
		Post2dWindowGeoDataGroupDataItem* item = m_itemNameMap.value(name);
		if (item != nullptr) {
			item->loadFromProjectMainFile(child);
		}
	}
}

void Post2dWindowGeoDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("GeoDataGroup");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

