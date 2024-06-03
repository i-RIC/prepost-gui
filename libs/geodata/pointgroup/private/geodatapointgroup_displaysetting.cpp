#include "geodatapointgroup_displaysetting.h"
#include "geodatapointgroup_scalesizepair.h"

#include <QBuffer>
#include <QByteArray>
#include <QDomElement>
#include <QXmlStreamWriter>

GeoDataPointGroup::DisplaySetting::DisplaySetting() :
	CompositeContainer {&mapping, &shape, &color, &opacity, &pointSize, &imageMaxSize},
	mapping {"mapping", Mapping::Value},
	shape {"shape", Shape::Point},
	anchorPosition {"anchorPosition", AnchorPosition::BottomLeft},
	color {"color", Qt::black},
	opacity {"opacity", 50},
	pointSize {"pointSize", 3},
	image {},
	imageMaxSize {"imageMaxSize", 64}
{
	scaleSizePairs.push_back(ScaleSizePair(0, 100));
	scaleSizePairs.push_back(ScaleSizePair(1e6, 100));
}

GeoDataPointGroup::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting ()
{
	copyValue(s);
	image = s.image;
}

GeoDataPointGroup::DisplaySetting& GeoDataPointGroup::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	image = s.image;
	return *this;
}

XmlAttributeContainer& GeoDataPointGroup::DisplaySetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const DisplaySetting&> (c));
}

void GeoDataPointGroup::DisplaySetting::load(const QDomNode& node)
{
	CompositeContainer::load(node);

	scaleSizePairs.clear();

	auto elem = node.toElement();
	QString img = elem.attribute("image");
	if (! img.isNull()) {
		QByteArray buffer = img.toUtf8();
		image = QImage::fromData(QByteArray::fromBase64(buffer), "PNG");
	}

	const auto& children = node.childNodes();
	for (int i = 0; i < children.size(); ++i) {
		QDomNode itemNode = children.at(i);
		if (itemNode.nodeName() != "Item") {continue;}

		ScaleSizePair pair;
		pair.load(itemNode);
		scaleSizePairs.push_back(pair);
	}
}

void GeoDataPointGroup::DisplaySetting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	if (! image.isNull()) {
		QByteArray bytes;
		QBuffer buffer(&bytes);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG");

		writer.writeAttribute("image", bytes.toBase64());
	}

	for (const auto& pair : scaleSizePairs) {
		writer.writeStartElement("Item");
		pair.save(writer);
		writer.writeEndElement();
	}
}
