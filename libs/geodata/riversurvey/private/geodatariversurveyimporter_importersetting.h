#ifndef GEODATARIVERSURVEYIMPORTER_IMPORTERSETTING_H
#define GEODATARIVERSURVEYIMPORTER_IMPORTERSETTING_H

#include "../geodatariversurveyimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/boolcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/stringcontainer.h>

class GeoDataRiverSurveyImporter::ImporterSetting : public GeoDataImporterSetting
{
	Q_OBJECT

public:
	ImporterSetting();

	EnumContainerT<GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting> cpSetting;
	StringContainer csvFilename;
	BoolContainer allNamesAreNumber;
	BoolContainer reverseOrder;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	std::vector<Item> customItems() const override;
};

#endif // GEODATARIVERSURVEYIMPORTER_IMPORTERSETTING_H
