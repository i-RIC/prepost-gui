#include "preprocessorcustommappingsetting.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QRegExp>

PreProcessorCustomMappingSetting::PreProcessorCustomMappingSetting()
{}

void PreProcessorCustomMappingSetting::load(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	QDomNamedNodeMap atts = elem.attributes();
	QRegExp attExp(attName("att_(.+)"));
	QRegExp bcExp(attName("bc_(.+)"));
	for (int i = 0; i < atts.count(); ++i) {
		QDomNode att = atts.item(i);
		QString name = att.nodeName();
		if (attExp.indexIn(name)) {
			// matched!
			std::string aName = iRIC::toStr(attExp.cap(1));
			bool val = iRIC::getBooleanAttribute(node, name);
			attSettings.insert(aName, val);
		} else if (bcExp.indexIn(name)) {
			// mached!
			std::string bcName = iRIC::toStr(attExp.cap(1));
			bool val = iRIC::getBooleanAttribute(node, name);
			bcSettings.insert(bcName, val);
		}
	}
}

void PreProcessorCustomMappingSetting::save(QXmlStreamWriter& writer) const
{
	for (auto it = attSettings.begin(); it != attSettings.end(); ++it) {
		std::string name = it.key();
		bool val = it.value();
		QString aName = QString("att_%1").arg(name.c_str());
		iRIC::setBooleanAttribute(writer, attName(aName), val);
	}
	for (auto it = bcSettings.begin(); it != bcSettings.end(); ++it) {
		std::string name = it.key();
		bool val = it.value();
		QString aName = QString("bc_%1").arg(name.c_str());
		iRIC::setBooleanAttribute(writer, attName(aName), val);
	}
}
