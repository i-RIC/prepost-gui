#include "colormapcustomsetting.h"

#include <QColor>
#include <QDomNode>
#include <QXmlStreamWriter>

ColorMapCustomSetting::ColorMapCustomSetting():
	type {"type", Type::ThreeColors},
	maxColor {"maxColor", Qt::red},
	midColor {"midColor", Qt::white},
	minColor {"minColor", Qt::blue},
	midValue {"midValue", 0},
	arbitrarySettings {}
{}

void ColorMapCustomSetting::load(const QDomNode& node)
{
	type.load(node);
	maxColor.load(node);
	minColor.load(node);
	midColor.load(node);
	midValue.load(node);

	arbitrarySettings.clear();

	if (type == Type::Arbitrary) {
		QDomNodeList cols = node.childNodes();
		for (int i = 0; i < cols.count(); ++i) {
			QDomNode colNode = cols.at(i);
			if (colNode.nodeName() != "Color") {continue;}

			ColorMapCustomSettingColor cc;
			cc.load(colNode);
			arbitrarySettings.push_back(cc);
		}
	}
}

void ColorMapCustomSetting::save(QXmlStreamWriter& writer) const
{
	type.save(writer);
	maxColor.save(writer);
	minColor.save(writer);
	midColor.save(writer);
	midValue.save(writer);

	if (type == Type::Arbitrary) {
		for (auto cc : arbitrarySettings) {
			writer.writeStartElement("Color");
			cc.save(writer);
			writer.writeEndElement();
		}
	}
}
