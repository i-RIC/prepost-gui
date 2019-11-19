#ifndef GEODATAPOLYGONGROUPSHPIMPORTER_H
#define GEODATAPOLYGONGROUPSHPIMPORTER_H

#include "geodatapolygongroupshpimportersettingdialog.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

class GeoDataPolygonGroupShpImporter : public GeoDataImporter
{
	Q_OBJECT

private:
	struct PolygonShapeInfo {
		int item;
		int region;
		std::vector<int> holes;
	};

public:
	GeoDataPolygonGroupShpImporter(GeoDataCreator* creator);
	~GeoDataPolygonGroupShpImporter() override;

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	static std::vector<PolygonShapeInfo> buildPolygonShapeInfos(const std::string& shpFileName);

	GeoDataPolygonGroupShpImporterSettingDialog::NameSetting m_nameSetting;
	int m_nameAttribute;
	GeoDataPolygonGroupShpImporterSettingDialog::ValueSetting m_valueSetting;
	std::vector<PolygonShapeInfo> m_shapeInfos;
	int m_valueAttribute;
	QVariant m_specifiedValue;
};

#endif // GEODATAPOLYGONGROUPSHPIMPORTER_H
