#ifndef GEODATARIVERSURVEYMLITIMPORTER_IMPORTERSETTING_H
#define GEODATARIVERSURVEYMLITIMPORTER_IMPORTERSETTING_H

#include "../geodatariversurveymlitimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/boolcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/stringcontainer.h>

class GeoDataRiverSurveyMlitImporter::ImporterSetting : public GeoDataImporterSetting
{
	Q_OBJECT

public:
	ImporterSetting();

	EnumContainerT<GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting> cpSetting;
	StringContainer csvFilename;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	std::vector<Item> customItems() const override;
};

#endif // GEODATARIVERSURVEYMLITIMPORTER_IMPORTERSETTING_H
