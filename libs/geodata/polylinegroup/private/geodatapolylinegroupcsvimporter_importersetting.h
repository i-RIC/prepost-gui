#ifndef GEODATAPOLYLINEGROUPCSVIMPORTER_IMPORTERSETTING_H
#define GEODATAPOLYLINEGROUPCSVIMPORTER_IMPORTERSETTING_H

#include "../geodatapolylinegroupcsvimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/stringcontainer.h>

class GeoDataPolyLineGroupCsvImporter::ImporterSetting : public GeoDataImporterSetting
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

#endif // GEODATAPOLYLINEGROUPCSVIMPORTER_IMPORTERSETTING_H
