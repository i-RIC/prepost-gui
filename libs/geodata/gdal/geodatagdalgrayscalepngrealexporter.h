#ifndef GEODATAGDALGRAYSCALEPNGREALEXPORTER_H
#define GEODATAGDALGRAYSCALEPNGREALEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataGdalReal;

class GeoDataGdalGrayscalePngRealExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataGdalGrayscalePngRealExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

private:
	bool doExportPng(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool doExportPgw(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool doExportMeta(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
};

#endif // GEODATAGDALGRAYSCALEPNGREALEXPORTER_H
