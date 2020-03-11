#ifndef GEODATANETCDFGDALIMPORTER_H
#define GEODATANETCDFGDALIMPORTER_H

#include "gd_netcdf_global.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <vector>

class CoordinateSystem;
class GeoDataNetcdf;
class GeoDataNetcdfFileNameMatcher;

class GDALDataset;
class GDALRasterBand;
class OGRSpatialReference;
class GeoDataNetcdf;

class GD_NETCDF_EXPORT GeoDataNetcdfGdalImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	enum class Mode {
		Single,
		Time
	};

	GeoDataNetcdfGdalImporter(GeoDataCreator* creator);
	virtual ~GeoDataNetcdfGdalImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private slots:
	void cancel();

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	bool doInitForSingleMode(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	bool doInitForTimeMode(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);

	bool importDataForSingleMode(GeoData* data, QWidget* w);
	bool importDataForTimeMode(GeoData* data, QWidget* w);

	bool setMode(SolverDefinitionGridAttribute* condition, QWidget* w);
	bool setCoordinateSystem(const QString& filename, GDALDataset* dataset, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w);
	bool setTransform(GDALDataset* dataset);
	void setupCoordinates(GeoDataNetcdf* data, GDALRasterBand* band);
	bool setupFileNamePattern(const QString &filename, QWidget *w);
	bool setupFilenames(const QString &filename, QWidget* w);
	void clear();

	virtual int outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataNetcdf* data) = 0;
	virtual int outputValuesWithTime(int ncid, int varId, int timeId, GDALRasterBand* band, GeoDataNetcdf* data) = 0;

	double m_transform[6];
	std::vector<QString> m_filenames;
	CoordinateSystem* m_coordinateSystem;
	Mode m_mode;
	GeoDataNetcdfFileNameMatcher* m_matcher;
	bool m_canceled;
};

#endif // GEODATANETCDFGDALIMPORTER_H
