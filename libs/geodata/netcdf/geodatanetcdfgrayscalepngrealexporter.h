#ifndef GEODATANETCDFGRAYSCALEPNGREALEXPORTER_H
#define GEODATANETCDFGRAYSCALEPNGREALEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataNetcdfReal;

class GeoDataNetcdfGrayscalePngRealExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataNetcdfGrayscalePngRealExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

private:
	bool doExportPng(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w);
	bool doExportPgw(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w);
	bool doExportMeta(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w);
};

#endif // GEODATANETCDFGRAYSCALEPNGREALEXPORTER_H
