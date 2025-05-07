#include "geodatapolydatagroupshpimporter_importersetting.h"

#include <QTextCodec>

GeoDataPolyDataGroupShpImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting(),
	nameSetting {"nameSetting", GeoDataPolyDataGroupShpImporterSettingDialog::NameSetting::nsAuto},
	nameAttribute {"nameAttribute"},
	valueSetting {"valueSetting", GeoDataPolyDataGroupShpImporterSettingDialog::ValueSetting::vsSpecify},
	valueAttribute {"valueAttribute"},
	specifiedValue {"specifiedValue"},
	codecName {"codecName"},
	csName {"csName"}
{}

void GeoDataPolyDataGroupShpImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	nameSetting.load(node);
	nameAttribute.load(node);
	valueSetting.load(node);
	valueAttribute.load(node);
	specifiedValue.load(node);
	codecName.load(node);
	csName.load(node);
}

void GeoDataPolyDataGroupShpImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	nameSetting.save(writer);
	nameAttribute.save(writer);
	valueSetting.save(writer);
	valueAttribute.save(writer);
	specifiedValue.save(writer);
	codecName.save(writer);
	csName.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataPolyDataGroupShpImporter::ImporterSetting::customItems() const
{
	QString nameSettingStr;
	QString valueSettingStr;

	if (nameSetting.value() == GeoDataPolyDataGroupShpImporterSettingDialog::NameSetting::nsAuto) {
		nameSettingStr = tr("Name is set automatically (ex. PolyData1)");
	} else {
		nameSettingStr = tr("Load from Shapefile attribute");
	}

	if (valueSetting.value() == GeoDataPolyDataGroupShpImporterSettingDialog::ValueSetting::vsSpecify) {
		valueSettingStr = tr("Specify value");
	} else {
		valueSettingStr = tr("Load from Shapefile attribute");
	}

	std::vector<GeoDataImporterSetting::Item> ret;

	auto codec = QTextCodec::codecForName(codecName.value().toLatin1());
	ret.push_back(Item({tr("Encoding"), codec->name()}));
	ret.push_back(Item({tr("Coordinate System"), csName}));
	ret.push_back(Item({tr("Name setting"), nameSettingStr}));
	ret.push_back(Item({tr("Name import target attribute"), nameAttribute}));
	ret.push_back(Item({tr("Value setting"), valueSettingStr}));
	ret.push_back(Item({tr("Value import target attribute"), valueAttribute}));
	ret.push_back(Item({tr("Specified value"), specifiedValue}));

	return ret;
}
