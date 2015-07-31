#include "preprocessorcustommappingsetting.h"

#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QRegExp>

PreProcessorCustomMappingSetting::PreProcessorCustomMappingSetting()
{
}

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
			QString aName = attExp.cap(1);
			bool val = iRIC::getBooleanAttribute(node, name);
			attSettings.insert(aName, val);
		} else if (bcExp.indexIn(name)) {
			// mached!
			QString bcName = attExp.cap(1);
			bool val = iRIC::getBooleanAttribute(node, name);
			bcSettings.insert(bcName, val);
		}
	}
}

void PreProcessorCustomMappingSetting::save(QXmlStreamWriter& writer) const
{
	for (auto it = attSettings.begin(); it != attSettings.end(); ++it) {
		QString name = it.key();
		bool val = it.value();
		QString aName = QString("att_%1").arg(name);
		iRIC::setBooleanAttribute(writer, attName(aName), val);
	}
	for (auto it = bcSettings.begin(); it != bcSettings.end(); ++it) {
		QString name = it.key();
		bool val = it.value();
		QString aName = QString("bc_%1").arg(name);
		iRIC::setBooleanAttribute(writer, attName(aName), val);
	}
}
