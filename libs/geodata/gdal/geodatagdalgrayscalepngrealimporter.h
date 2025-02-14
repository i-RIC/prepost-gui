#ifndef GEODATAGDALGRAYSCALEPNGREALIMPORTER_H
#define GEODATAGDALGRAYSCALEPNGREALIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystem;
class GeoDataGdalReal;

class GeoDataGdalGrayscalePngRealImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataGdalGrayscalePngRealImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	bool importPng(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool importPgw(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool importMeta(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool setCs(PreProcessorGeoDataGroupDataItemI* item, QWidget* w);

	CoordinateSystem* m_coordinateSystem;
};

#endif // GEODATAGDALGRAYSCALEPNGREALIMPORTER_H
