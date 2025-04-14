#ifndef GEODATAPOINTMAPREALTEXTIMPORTER_IMPORTERSETTING_H
#define GEODATAPOINTMAPREALTEXTIMPORTER_IMPORTERSETTING_H

#include "../geodatapointmaprealtextimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/boolcontainer.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>

class GeoDataPointmapRealTextImporter::ImporterSetting : public GeoDataImporterSetting
{
	Q_OBJECT

public:
	ImporterSetting();

	StringContainer codecName;
	StringContainer csName;

	BoolContainer delimiterComma;
	BoolContainer delimiterTab;
	BoolContainer delimiterSpace;
	BoolContainer delimiterColon;
	BoolContainer delimiterSemicolon;
	BoolContainer delimiterOther;
	StringContainer otherDelimiter;

	StringContainer quoteCharacter;
	StringContainer escapeCharacter;

	IntContainer headerLines;
	IntContainer fieldX;
	IntContainer fieldY;
	IntContainer fieldValue;

	IntContainer skipRate;

	LineParser* buildParser();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	std::vector<Item> customItems() const override;
};

#endif // GEODATAPOINTMAPREALTEXTIMPORTER_IMPORTERSETTING_H
