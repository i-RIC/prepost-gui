#include "geodatapointgroup_displaysetting.h"

#include <QBuffer>
#include <QByteArray>
#include <QDomElement>
#include <QXmlStreamWriter>

GeoDataPointGroup::DisplaySetting::DisplaySetting() :
	CompositeContainer {&mapping, &shape, &color, &opacity, &pointSize, &imageMaxSize},
	mapping {"mapping", Mapping::Value},
	shape {"shape", Shape::Point},
	color {"color", Qt::black},
	opacity {"opacity", 50},
	pointSize {"pointSize", 3},
	image {},
	imageMaxSize {"imageMaxSize", 64}
{}

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

	auto elem = node.toElement();
	QString img = elem.attribute("image");
	if (! img.isNull()) {
		QByteArray buffer = img.toUtf8();
		image = QImage::fromData(QByteArray::fromBase64(buffer), "PNG");
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
}
