#ifndef GEODATAPOLYDATAGROUPSHPIMPORTER_IMPORTERSETTING_H
#define GEODATAPOLYDATAGROUPSHPIMPORTER_IMPORTERSETTING_H

#include "../geodatapolydatagroupshpimporter.h"

#include <guicore/pre/geodata/geodataimportersetting.h>
#include <misc/enumcontainert.h>
#include <misc/stringcontainer.h>

class GeoDataPolyDataGroupShpImporter::ImporterSetting : public GeoDataImporterSetting
{
	Q_OBJECT

public:
	ImporterSetting();

	EnumContainerT<GeoDataPolyDataGroupShpImporterSettingDialog::NameSetting> nameSetting;
	StringContainer nameAttribute;
	EnumContainerT<GeoDataPolyDataGroupShpImporterSettingDialog::ValueSetting> valueSetting;
	StringContainer valueAttribute;
	StringContainer specifiedValue;
	StringContainer codecName;
	StringContainer csName;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	std::vector<Item> customItems() const override;
};

#endif // GEODATAPOLYDATAGROUPSHPIMPORTER_IMPORTERSETTING_H
