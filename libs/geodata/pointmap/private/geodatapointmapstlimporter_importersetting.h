#ifndef GEODATAPOINTMAPSTLIMPORTER_IMPORTERSETTING_H
#define GEODATAPOINTMAPSTLIMPORTER_IMPORTERSETTING_H

#include "../geodatapointmapstlimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/stringcontainer.h>

class GeoDataPointmapSTLImporter::ImporterSetting : public GeoDataImporterSetting
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

#endif // GEODATAPOINTMAPSTLIMPORTER_IMPORTERSETTING_H
