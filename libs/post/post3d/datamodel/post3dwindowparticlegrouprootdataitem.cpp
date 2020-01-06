#include "post3dwindowparticlegrouprootdataitem.h"
#include "post3dwindowparticlegrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Post3dWindowParticleGroupRootDataItem::Post3dWindowParticleGroupRootDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Particles"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
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

void Post3dWindowParticleGroupRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post3dWindowParticleGroupTopDataItem*>(c);
		QDomNode cNode = iRIC::getChildNodeWithAttribute(node, "ParticleGroup", "name", gItem->name().c_str());
		if (! cNode.isNull()) {
			gItem->loadFromProjectMainFile(cNode);
		}
	}
}

void Post3dWindowParticleGroupRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post3dWindowParticleGroupTopDataItem*>(c);
		writer.writeStartElement("ParticleGroup");
		writer.writeAttribute("name", gItem->name().c_str());
		gItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
