#include "post3dwindowparticlegrouprootdataitem.h"
#include "post3dwindowparticlegrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Post3dWindowParticleGroupRootDataItem::Post3dWindowParticleGroupRootDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Particles"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	const auto& map = cont->particleGroupMap();

	for (auto pair : map) {
		auto gItem = new Post3dWindowParticleGroupTopDataItem(pair.first, this);
		gItem->setup();
		m_childItems.push_back(gItem);
	}
}

Post3dWindowParticleGroupRootDataItem::~Post3dWindowParticleGroupRootDataItem()
{}

void Post3dWindowParticleGroupRootDataItem::update()
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post3dWindowParticleGroupTopDataItem*>(c);
		gItem->update();
	}
}

Post3dWindowZoneDataItem* Post3dWindowParticleGroupRootDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}

void Post3dWindowParticleGroupRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticleGroupTopDataItem*>(child);
		QDomNode cNode = iRIC::getChildNodeWithAttribute(node, "ParticleGroup", "name", item->name().c_str());
		if (! cNode.isNull()) {
			item->loadFromProjectMainFile(cNode);
		}
	}
}

void Post3dWindowParticleGroupRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticleGroupTopDataItem*>(child);
		writer.writeStartElement("ParticleGroup");
		writer.writeAttribute("name", item->name().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
