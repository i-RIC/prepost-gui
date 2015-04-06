#ifndef PREPROCESSORCUSTOMMAPPINGSETTING_H
#define PREPROCESSORCUSTOMMAPPINGSETTING_H

#include <misc/xmlattributecontainer.h>
#include <QMap>
#include <QString>

class PreProcessorCustomMappingSetting : XmlAttributeContainer
{
public:
	PreProcessorCustomMappingSetting();

	QMap<QString, bool> attSettings;
	QMap<QString, bool> bcSettings;

	void load(const QDomNode& node);
	void save(QXmlStreamWriter& writer) const;
};

#endif // PREPROCESSORCUSTOMMAPPINGSETTING_H
