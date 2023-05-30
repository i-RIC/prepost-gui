#include "post3dwindownodevectorparticlegroupstructureddataitem_setting.h"

#include <postbase/particle/particlesettingcontainer.h>

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::StartPosition() :
	CompositeContainer({&skipOrSubdivide, &face, &color, &pointSize}),
	skipOrSubdivide {},
	face {},
	color {"color", Qt::black},
	pointSize {"pointSize", ParticleSettingContainer::DEFAULT_SIZE}
{}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::StartPosition(const StartPosition& pos) :
	StartPosition {}
{
	copyValue(pos);
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition& Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::operator=(const StartPosition& pos)
{
	copyValue(pos);
	return *this;
}

XmlAttributeContainer& Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const StartPosition&> (setting));
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::Setting() :
	CompositeContainer({})
{}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting& Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Setting&> (setting));
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::copyValue(const XmlAttributeContainer& setting)
{
	const auto& setting2 = dynamic_cast<const Setting&> (setting);
	CompositeContainer::copyValue(setting2);

	startPositions = setting2.startPositions;
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	startPositions.clear();

	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		StartPosition pos;
		pos.load(child);
		startPositions.push_back(pos);
	}
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	for (const auto& pos : startPositions) {
		writer.writeStartElement("StartPosition");
		pos.save(writer);
		writer.writeEndElement();
	}
}
