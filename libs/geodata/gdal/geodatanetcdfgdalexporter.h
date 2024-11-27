#ifndef GEODATANETCDFGDALEXPORTER_H
#define GEODATANETCDFGDALEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

#include <gdal_priv.h>

class GeoDataNetcdf;

class GDALRasterBand;

class GeoDataNetcdfGdalExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataNetcdfGdalExporter(GeoDataCreator* creator);
	virtual ~GeoDataNetcdfGdalExporter();

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

	virtual GDALDataType gdalDataType() const = 0;
	virtual void copyData(GeoDataNetcdf* netcdf, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band) = 0;
};

#endif // GEODATANETCDFGDALEXPORTER_H
