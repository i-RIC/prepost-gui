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
	GeoDataImporterSetting* createSetting() const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	bool importPng(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool importPgw(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool importMeta(GeoDataGdalReal* gdal, const QString& filename, QWidget* w);
	bool setCs(PreProcessorGeoDataGroupDataItemI* item, QWidget* w);

protected:
	int m_tgtISize;
	int m_tgtJSize;

	double m_srcTransform[6];
	double m_tgtTransform[6];
	std::vector<int> m_matrix;

	CoordinateSystem* m_coordinateSystem;
	PreProcessorGeoDataGroupDataItemI* m_item;

	class ImporterSetting;
};

#endif // GEODATAGDALGRAYSCALEPNGREALIMPORTER_H
