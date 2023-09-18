#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowparticlegrouprootdataitem.h"
#include "post2dwindowparticlegrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

Post2dWindowParticleGroupRootDataItem::Post2dWindowParticleGroupRootDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Particles"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	const auto& map = cont->particleGroupMap();

	for (auto pair : map) {
		auto gItem = new Post2dWindowParticleGroupTopDataItem(pair.first, this);
		gItem->setup();
		m_childItems.push_back(gItem);
	}
}

Post2dWindowParticleGroupRootDataItem::~Post2dWindowParticleGroupRootDataItem()
{}

void Post2dWindowParticleGroupRootDataItem::update()
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post2dWindowParticleGroupTopDataItem*>(c);
		gItem->update();
	}
}

Post2dWindowCalculationResultDataItem* Post2dWindowParticleGroupRootDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*>(parent());
}

Post2dWindowZoneDataItem* Post2dWindowParticleGroupRootDataItem::zoneDataItem() const
{
	return resultDataItem()->zoneDataItem();
}

void Post2dWindowParticleGroupRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post2dWindowParticleGroupTopDataItem*>(c);
		QDomNode cNode = iRIC::getChildNodeWithAttribute(node, "ParticleGroup", "name", gItem->name().c_str());
		if (! cNode.isNull()) {
			gItem->loadFromProjectMainFile(cNode);
		}
	}
}

void Post2dWindowParticleGroupRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto c : m_childItems) {
		auto gItem = dynamic_cast<Post2dWindowParticleGroupTopDataItem*>(c);
		writer.writeStartElement("ParticleGroup");
		writer.writeAttribute("name", gItem->name().c_str());
		gItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
