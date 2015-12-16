#ifndef PREPROCESSORCUSTOMMAPPINGSETTING_H
#define PREPROCESSORCUSTOMMAPPINGSETTING_H

#include <misc/xmlattributecontainer.h>
#include <QMap>
#include <QString>

class PreProcessorCustomMappingSetting : public XmlAttributeContainer
{
public:
	PreProcessorCustomMappingSetting();

	QMap<std::string, bool> attSettings;
	QMap<std::string, bool> bcSettings;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;
};

#endif // PREPROCESSORCUSTOMMAPPINGSETTING_H
