#ifndef GEODATAPOLYDATAGROUPSHPIMPORTER_H
#define GEODATAPOLYDATAGROUPSHPIMPORTER_H

#include "gd_polydatagroup_global.h"
#include "geodatapolydatagroupshpimportersettingdialog.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

#include <shapefil.h>

class QTextCodec;

class GD_POLYDATAGROUP_EXPORT GeoDataPolyDataGroupShpImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPolyDataGroupShpImporter(const std::string& name, const QString& caption, GeoDataCreator* creator);
	~GeoDataPolyDataGroupShpImporter() override;

	const QStringList acceptableExtensions() override;

protected:
	static QVariant readData(DBFHandle handle, int dataid, int fieldid, QTextCodec* codec);

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

protected:
	GeoDataPolyDataGroupShpImporterSettingDialog::NameSetting m_nameSetting;
	int m_nameAttribute;
	GeoDataPolyDataGroupShpImporterSettingDialog::ValueSetting m_valueSetting;
	// std::vector<PolygonShapeInfo> m_shapeInfos;
	int m_valueAttribute;
	QVariant m_specifiedValue;
	QTextCodec* m_codec;
};

#endif // GEODATAPOLYDATAGROUPSHPIMPORTER_H
