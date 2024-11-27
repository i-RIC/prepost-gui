#ifndef GEODATANETCDFGRAYSCALEPNGREALIMPORTER_H
#define GEODATANETCDFGRAYSCALEPNGREALIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystem;
class GeoDataNetcdfReal;

class GeoDataNetcdfGrayscalePngRealImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataNetcdfGrayscalePngRealImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	bool importPng(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w);
	bool importPgw(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w);
	bool importMeta(GeoDataNetcdfReal* netcdf, const QString& filename, QWidget* w);
	bool setCs(PreProcessorGeoDataGroupDataItemI* item, QWidget* w);

	CoordinateSystem* m_coordinateSystem;
};

#endif // GEODATANETCDFGRAYSCALEPNGREALIMPORTER_H
