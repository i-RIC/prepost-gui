#ifndef GEODATAPOLYDATAGROUPSHPIMPORTER_H
#define GEODATAPOLYDATAGROUPSHPIMPORTER_H

#include "gd_polydatagroup_global.h"
#include "geodatapolydatagroupshpimportersettingdialog.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

#include <shapefil.h>

class CoordinateSystemConverter;

class QTextCodec;

class GD_POLYDATAGROUP_EXPORT GeoDataPolyDataGroupShpImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPolyDataGroupShpImporter(const std::string& name, const QString& caption, GeoDataCreator* creator);
	~GeoDataPolyDataGroupShpImporter() override;

	const QStringList acceptableExtensions() override;
	GeoDataImporterSetting* createSetting() const override;

protected:
	static QVariant readData(DBFHandle handle, int dataid, int fieldid, QTextCodec* codec);

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

protected:
	GeoDataPolyDataGroupShpImporterSettingDialog::NameSetting m_nameSetting;
	int m_nameAttribute;
	QString m_nameAttributeName;
	GeoDataPolyDataGroupShpImporterSettingDialog::ValueSetting m_valueSetting;
	int m_valueAttribute;
	QString m_valueAttributeName;
	QVariant m_specifiedValue;
	QTextCodec* m_codec;
	CoordinateSystemConverter* m_converter;

	class ImporterSetting;
};

#endif // GEODATAPOLYDATAGROUPSHPIMPORTER_H
