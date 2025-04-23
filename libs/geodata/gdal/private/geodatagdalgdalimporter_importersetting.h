#ifndef GEODATAGDALGDALIMPORTER_IMPORTERSETTING_H
#define GEODATAGDALGDALIMPORTER_IMPORTERSETTING_H

#include "../geodatagdalgdalimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/boolcontainer.h>
#include <misc/stringcontainer.h>

class GeoDataGdalGdalImporter::ImporterSetting : public GeoDataImporterSetting
{
	Q_OBJECT

public:
	ImporterSetting();

	BoolContainer timeMode;
	StringContainer csName;
	StringContainer fileNamePattern;
	StringContainer timeZone;
	StringContainer fileNames;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	std::vector<Item> customItems() const override;};

#endif // GEODATAGDALGDALIMPORTER_IMPORTERSETTING_H
