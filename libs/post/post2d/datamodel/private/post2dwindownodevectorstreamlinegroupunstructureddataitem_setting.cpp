#include "post2dwindownodevectorstreamlinegroupunstructureddataitem_setting.h"


Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::StartPosition::StartPosition() :
	CompositeContainer({&point1, &point2, &pointsSet, &numberOfPoints, &color, &width}),
	point1 {"point1"},
	point2 {"point2"},
	pointsSet {"pointsSet", false},
	numberOfPoints {"numberOfPoints"},
	color {"color", Qt::black},
	width {"width", 1}
{}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::StartPosition::StartPosition(const StartPosition& pos) :
	StartPosition {}
{
	copyValue(pos);
}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::StartPosition& Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::StartPosition::operator=(const StartPosition& pos)
{
	copyValue(pos);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::StartPosition::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const StartPosition&> (setting));
}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::Setting() :
	CompositeContainer({&region})
{}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting& Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Setting&> (setting));
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::copyValue(const XmlAttributeContainer& setting)
{
	const auto& setting2 = dynamic_cast<const Setting&> (setting);
	CompositeContainer::copyValue(setting2);

	startPositions = setting2.startPositions;
}

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::load(const QDomNode& node)
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

void Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	for (const auto& pos : startPositions) {
		writer.writeStartElement("StartPosition");
		pos.save(writer);
		writer.writeEndElement();
	}
}
