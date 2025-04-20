#ifndef GEODATAGDALNETCDFIMPORTER_IMPORTERSETTING_H
#define GEODATAGDALNETCDFIMPORTER_IMPORTERSETTING_H

#include "../geodatagdalnetcdfimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/stringcontainer.h>

class GeoDataGdalNetcdfImporter::ImporterSetting : public GeoDataImporterSetting
{
public:
	ImporterSetting();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	std::vector<Item> customItems() const override;
};

#endif // GEODATAGDALNETCDFIMPORTER_IMPORTERSETTING_H
