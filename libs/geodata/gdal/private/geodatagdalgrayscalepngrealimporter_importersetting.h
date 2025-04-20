#ifndef GEODATAGDALGRAYSCALEPNGREALIMPORTER_IMPORTERSETTING_H
#define GEODATAGDALGRAYSCALEPNGREALIMPORTER_IMPORTERSETTING_H

#include "../geodatagdalgrayscalepngrealimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/stringcontainer.h>

class GeoDataGdalGrayscalePngRealImporter::ImporterSetting : public GeoDataImporterSetting
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

#endif // GEODATAGDALGRAYSCALEPNGREALIMPORTER_IMPORTERSETTING_H
