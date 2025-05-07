#ifndef GEODATAPOINTMAPLANDXMLIMPORTER_IMPORTERSETTING_H
#define GEODATAPOINTMAPLANDXMLIMPORTER_IMPORTERSETTING_H

#include "../geodatapointmaplandxmlimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/stringcontainer.h>

class GeoDataPointmapLandXmlImporter::ImporterSetting : public GeoDataImporterSetting
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

#endif // GEODATAPOINTMAPLANDXMLIMPORTER_IMPORTERSETTING_H
