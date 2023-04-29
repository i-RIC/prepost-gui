#include "post2dwindownodevectorstreamlinegroupstructureddataitem_setting.h"

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::StartPosition() :
	CompositeContainer({&skipOrSubdivide, &range, &color, &width}),
	skipOrSubdivide {},
	range {},
	color {"color", Qt::black},
	width {"width", 1}
{}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::StartPosition(const StartPosition& pos) :
	StartPosition {}
{
	copyValue(pos);
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition& Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::operator=(const StartPosition& pos)
{
	copyValue(pos);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const StartPosition&> (setting));
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::Setting() :
	CompositeContainer({&region})
{}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting& Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Setting&> (setting));
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::copyValue(const XmlAttributeContainer& setting)
{
	const auto& setting2 = dynamic_cast<const Setting&> (setting);
	CompositeContainer::copyValue(setting2);

	startPositions = setting2.startPositions;
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::load(const QDomNode& node)
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

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	for (const auto& pos : startPositions) {
		writer.writeStartElement("StartPosition");
		pos.save(writer);
		writer.writeEndElement();
	}
}
