#ifndef GEODATANETCDFGDALIMPORTER_H
#define GEODATANETCDFGDALIMPORTER_H

#include "gd_netcdf_global.h"
#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystem;
class GDALDataset;
class GDALRasterBand;
class OGRSpatialReference;
class GeoDataNetcdf;

class GD_NETCDF_EXPORT GeoDataNetcdfGdalImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataNetcdfGdalImporter(GeoDataCreator* creator);
	virtual ~GeoDataNetcdfGdalImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	virtual int outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataNetcdf* data) = 0;
	GDALDataset* m_dataset;
	OGRSpatialReference* m_sr;
	double m_transform[6];
	CoordinateSystem* m_coordinateSystem;
	int m_count;
};

#endif // GEODATANETCDFGDALIMPORTER_H
