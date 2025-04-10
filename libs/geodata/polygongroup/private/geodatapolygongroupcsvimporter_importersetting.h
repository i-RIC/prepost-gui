#ifndef GEODATAPOLYGONGROUPCSVIMPORTER_IMPORTERSETTING_H
#define GEODATAPOLYGONGROUPCSVIMPORTER_IMPORTERSETTING_H

#include "../geodatapolygongroupcsvimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/stringcontainer.h>

class GeoDataPolygonGroupCsvImporter::ImporterSetting : public GeoDataImporterSetting
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

#endif // GEODATAPOLYGONGROUPCSVIMPORTER_IMPORTERSETTING_H
