#include "post2dwindownodevectorparticlegroupunstructureddataitem_setting.h"


Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::StartPosition::StartPosition() :
	CompositeContainer({&point1, &point2, &pointsSet, &numberOfPoints, &color, &pointSize}),
	point1 {"point1"},
	point2 {"point2"},
	pointsSet {"pointsSet", false},
	numberOfPoints {"numberOfPoints"},
	color {"color", Qt::black},
	pointSize {"pointSize", 1}
{}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::StartPosition::StartPosition(const StartPosition& pos) :
	StartPosition {}
{
	copyValue(pos);
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::StartPosition& Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::StartPosition::operator=(const StartPosition& pos)
{
	copyValue(pos);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::StartPosition::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const StartPosition&> (setting));
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::Setting() :
	CompositeContainer({&region})
{}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting& Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Setting&> (setting));
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::copyValue(const XmlAttributeContainer& setting)
{
	const auto& setting2 = dynamic_cast<const Setting&> (setting);
	CompositeContainer::copyValue(setting2);

	startPositions = setting2.startPositions;
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::load(const QDomNode& node)
{
	startPositions.clear();
	CompositeContainer::load(node);
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		StartPosition pos;
		pos.load(child);
		startPositions.push_back(pos);
	}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	for (const auto& pos : startPositions) {
		writer.writeStartElement("StartPosition");
		pos.save(writer);
		writer.writeEndElement();
	}
}
