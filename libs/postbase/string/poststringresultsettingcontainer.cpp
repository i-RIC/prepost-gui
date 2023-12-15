#include "poststringresultsettingcontainer.h"
#include "public/poststringresultsettingcontainer_imagebuilder.h"

#include <QSettings>

PostStringResultSettingContainer::PostStringResultSettingContainer() :
	CompositeContainer({&fontColor, &font, &backgroundColor, &imageSetting}),
	script {"script"},
	fontColor {"fontColor", Qt::black},
	font {"font"},
	backgroundColor {"backgroundColor", Qt::lightGray},
	imageSetting {},
	imageBuilder {new ImageBuilder {this}}
{
	QSettings settings;

	QFont f;
	f.fromString(settings.value("fontsetting/vis_stringresult", QString("MS UI Gothic,9,-1,5,50,0,0,0,0,0")).toString());
	font = f;

	imageSetting.setImageBuilder(imageBuilder);
}

PostStringResultSettingContainer::PostStringResultSettingContainer(const PostStringResultSettingContainer& s) :
	PostStringResultSettingContainer {}
{
	copyValue(s);
}

PostStringResultSettingContainer::~PostStringResultSettingContainer()
{
	delete imageBuilder;
}

PostStringResultSettingContainer& PostStringResultSettingContainer::operator=(const PostStringResultSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& PostStringResultSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const PostStringResultSettingContainer&> (c));
}

void PostStringResultSettingContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const PostStringResultSettingContainer&> (c);
	CompositeContainer::copyValue(c2);

	script = c2.script;
	arguments = c2.arguments;
}

void PostStringResultSettingContainer::load(const QDomNode& node)
{
	CompositeContainer::load(node);

	auto childList = node.childNodes();
	for (int i = 0; i < childList.size(); ++i) {
		auto c = childList.at(i);
		if (c.nodeName() == "Script") {
			auto scriptChildren = c.childNodes();
			for (int j = 0; j < scriptChildren.size(); ++j) {
				auto sc = scriptChildren.at(j);
				script = sc.nodeValue();
			}
		} else if (c.nodeName() == "Argument") {
			PostStringResultArgumentContainer arg;
			arg.load(c);
			arguments.push_back(arg);
		}
	}
}

void PostStringResultSettingContainer::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	writer.writeStartElement("Script");
	writer.writeCharacters(script);
	writer.writeEndElement();
	for (auto arg : arguments) {
		writer.writeStartElement("Argument");
		arg.save(writer);
		writer.writeEndElement();
	}
}

bool PostStringResultSettingContainer::importData(const QString& fileName)
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
	if (elem.nodeName() != "LabelSetting") {
		return false;
	}
	load(elem);
	return true;
}

bool PostStringResultSettingContainer::exportData(const QString& fileName)
{
	QFile f(fileName);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QXmlStreamWriter writer(&f);
	writer.writeStartDocument();
	writer.writeStartElement("LabelSetting");
	save(writer);
	writer.writeEndElement();
	writer.writeEndDocument();
	f.close();

	return true;
}
