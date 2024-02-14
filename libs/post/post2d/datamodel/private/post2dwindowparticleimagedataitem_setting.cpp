#include "post2dwindowparticleimagedataitem_setting.h"

#include <QBuffer>

Post2dWindowParticleImageDataItem::Setting::Setting() :
	CompositeContainer({&anchorPosition, &sizeTarget, &fillUpper, &fillLower, &opacity}),
	anchorPosition {"anchorPosition", AnchorPosition::Center},
	sizeTarget {"sizeTarget", SizeTarget::Width},
	fillUpper {"fillUppeer", true},
	fillLower {"fillLower", true},
	opacity {}
{
	valueSizePairs.push_back(ValueSizePair(0, 0));
	valueSizePairs.push_back(ValueSizePair(1000, 1000));

	image = QImage(":/libs/post/post2d/images/particleImage.png");
}

Post2dWindowParticleImageDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
	image = setting.image;
}

Post2dWindowParticleImageDataItem::Setting::~Setting()
{}

Post2dWindowParticleImageDataItem::Setting& Post2dWindowParticleImageDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	image = setting.image;
	return *this;
}

XmlAttributeContainer& Post2dWindowParticleImageDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}

void Post2dWindowParticleImageDataItem::Setting::load(const QDomNode& node)
{
	CompositeContainer::load(node);

	valueSizePairs.clear();

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

		ValueSizePair pair;
		pair.load(itemNode);
		valueSizePairs.push_back(pair);
	}
}

void Post2dWindowParticleImageDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);

	if (! image.isNull()) {
		QByteArray bytes;
		QBuffer buffer(&bytes);
		buffer.open(QIODevice::WriteOnly);
		image.save(&buffer, "PNG");

		writer.writeAttribute("image", bytes.toBase64());
	}

	for (const auto& pair : valueSizePairs) {
		writer.writeStartElement("Item");
		pair.save(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowParticleImageDataItem::Setting::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const Setting&> (c);
	CompositeContainer::copyValue(c2);

	valueSizePairs = c2.valueSizePairs;
}

bool Post2dWindowParticleImageDataItem::Setting::importData(const QString& fileName)
{
	QFile f(fileName);
	QDomDocument doc;

	QString errorStr;
	int errorLine;
	int errorColumn;

	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		return false;
	}
	auto elem = doc.documentElement();
	if (elem.nodeName() != "ParticleImageSetting") {
		return false;
	}
	load(elem);
	return true;
}

bool Post2dWindowParticleImageDataItem::Setting::exportData(const QString& fileName)
{
	QFile f(fileName);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QXmlStreamWriter writer(&f);
	writer.writeStartDocument();
	writer.writeStartElement("ParticleImageSetting");
	save(writer);
	writer.writeEndElement();
	writer.writeEndDocument();
	f.close();

	return true;
}
