#include "post2dwindownodevectorparticlegroupstructureddataitem_setting.h"

#include <postbase/particle/particlesettingcontainer.h>

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::StartPosition() :
	CompositeContainer({&skipOrSubdivide, &range, &color, &pointSize}),
	skipOrSubdivide {},
	range {},
	color {"color", Qt::black},
	pointSize {"pointSize", ParticleSettingContainer::DEFAULT_SIZE}
{}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::StartPosition(const StartPosition& pos) :
	StartPosition {}
{
	copyValue(pos);
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition& Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::operator=(const StartPosition& pos)
{
	copyValue(pos);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const StartPosition&> (setting));
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::Setting() :
	CompositeContainer({&region})
{}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting& Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Setting&> (setting));
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::copyValue(const XmlAttributeContainer& setting)
{
	const auto& setting2 = dynamic_cast<const Setting&> (setting);
	CompositeContainer::copyValue(setting2);

	startPositions = setting2.startPositions;
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		StartPosition pos;
		pos.load(child);
		startPositions.push_back(pos);
	}
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	for (const auto& pos : startPositions) {
		writer.writeStartElement("StartPosition");
		pos.save(writer);
		writer.writeEndElement();
	}
}
