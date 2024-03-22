#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowparticleimagetopdataitem.h"
#include "post2dwindowparticleimagedataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <misc/xmlsupport.h>

Post2dWindowParticleImageTopDataItem::Post2dWindowParticleImageTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Particles (image)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	const auto& map = cont->particleGroupImageMap();

	for (const auto& pair : map) {
		auto item = new Post2dWindowParticleImageDataItem(pair.first, pair.first.c_str(), this);
		m_childItems.push_back(item);
	}
}

Post2dWindowParticleImageTopDataItem::~Post2dWindowParticleImageTopDataItem()
{}

void Post2dWindowParticleImageTopDataItem::update()
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticleImageDataItem*>(c);
		item->update();
	}
}

Post2dWindowCalculationResultDataItem* Post2dWindowParticleImageTopDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowParticleImageTopDataItem::zoneDataItem() const
{
	return resultDataItem()->zoneDataItem();
}

void Post2dWindowParticleImageTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticleImageDataItem*>(c);
		QDomNode cNode = iRIC::getChildNodeWithAttribute(node, "ParticleGroupImage", "name", item->name().c_str());
		if (! cNode.isNull()) {
			item->loadFromProjectMainFile(cNode);
		}
	}
}

void Post2dWindowParticleImageTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticleImageDataItem*>(c);
		writer.writeStartElement("ParticleGroupImage");
		writer.writeAttribute("name", item->name().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
