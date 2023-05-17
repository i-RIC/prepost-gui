#include "post3dwindownodevectorstreamlinegroupstructureddataitem_setting.h"

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::StartPosition() :
	CompositeContainer({&skipOrSubdivide, &face, &color, &width}),
	skipOrSubdivide {},
	face {},
	color {"color", Qt::black},
	width {"width", 1}
{}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::StartPosition(const StartPosition& pos) :
	StartPosition {}
{
	copyValue(pos);
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition& Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::operator=(const StartPosition& pos)
{
	copyValue(pos);
	return *this;
}

XmlAttributeContainer& Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const StartPosition&> (setting));
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::Setting() :
	CompositeContainer({})
{}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting& Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Setting&> (setting));
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::copyValue(const XmlAttributeContainer& setting)
{
	const auto& setting2 = dynamic_cast<const Setting&> (setting);
	CompositeContainer::copyValue(setting2);

	startPositions = setting2.startPositions;
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::load(const QDomNode& node)
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

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	for (const auto& pos : startPositions) {
		writer.writeStartElement("StartPosition");
		pos.save(writer);
		writer.writeEndElement();
	}
}
