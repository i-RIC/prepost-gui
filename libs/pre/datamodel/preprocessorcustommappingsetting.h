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

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;
};

#endif // PREPROCESSORCUSTOMMAPPINGSETTING_H
