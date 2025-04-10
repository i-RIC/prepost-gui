#ifndef GEODATAPOINTGROUPCSVIMPORTER_IMPORTERSETTING_H
#define GEODATAPOINTGROUPCSVIMPORTER_IMPORTERSETTING_H

#include "../geodatapointgroupcsvimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/stringcontainer.h>

class GeoDataPointGroupCsvImporter::ImporterSetting : public GeoDataImporterSetting
{
	Q_OBJECT

public:
	ImporterSetting();

	StringContainer csName;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	std::vector<Item> customItems() const override;
};

#endif // GEODATAPOINTGROUPCSVIMPORTER_IMPORTERSETTING_H
