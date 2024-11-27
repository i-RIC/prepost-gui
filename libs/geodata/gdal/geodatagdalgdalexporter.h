#ifndef GEODATAGDALGDALEXPORTER_H
#define GEODATAGDALGDALEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

#include <gdal_priv.h>

class GeoDataGdal;

class GDALRasterBand;

class GeoDataGdalGdalExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataGdalGdalExporter(GeoDataCreator* creator);
	virtual ~GeoDataGdalGdalExporter();

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

	virtual GDALDataType gdalDataType() const = 0;
	virtual void copyData(GeoDataGdal* gdal, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band) = 0;
};

#endif // GEODATAGDALGDALEXPORTER_H
