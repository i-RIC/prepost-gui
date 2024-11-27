#ifndef GEODATAGDALGDALIMPORTER_H
#define GEODATAGDALGDALIMPORTER_H

#include "gd_gdal_global.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <QTimeZone>

#include <vector>

class CoordinateSystem;
class GeoDataGdal;
class GeoDataGdalFileNameMatcher;

class GDALDataset;
class GDALRasterBand;
class OGRSpatialReference;
class GeoDataGdal;

class GD_GDAL_EXPORT GeoDataGdalGdalImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	enum class Mode {
		Single,
		Time
	};

	GeoDataGdalGdalImporter(GeoDataCreator* creator);
	virtual ~GeoDataGdalGdalImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private slots:
	void cancel();

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	bool doInitForSingleMode(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);
	bool doInitForTimeMode(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);

	bool importDataForSingleMode(GeoDataGdal* data, QWidget* w);
	bool importDataForTimeMode(GeoDataGdal* data, QWidget* w);

	bool setMode(SolverDefinitionGridAttribute* condition, QWidget* w);
	bool setCoordinateSystem(const QString& filename, GDALDataset* dataset, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);
	bool setTransform(GDALDataset* dataset);
	void setupCoordinates(GeoDataGdal* data, GDALRasterBand* band);
	bool setupFileNamePattern(const QString &filename, QWidget *w);
	bool setupFilenames(const QString &filename, QWidget* w);
	void clear();

	virtual int outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataGdal* data) = 0;
	virtual int outputValuesWithTime(int ncid, int varId, int timeId, GDALRasterBand* band, GeoDataGdal* data) = 0;

	double m_transform[6];
	std::vector<QString> m_filenames;
	CoordinateSystem* m_coordinateSystem;
	QTimeZone m_timeZone;
	Mode m_mode;
	GeoDataGdalFileNameMatcher* m_matcher;
	bool m_canceled;
};

#endif // GEODATAGDALGDALIMPORTER_H
