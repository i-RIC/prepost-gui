#ifndef GEODATAPOLYGONGROUPCSVIMPORTER_H
#define GEODATAPOLYGONGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystemConverter;

class GeoDataPolygonGroupCsvImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPolygonGroupCsvImporter(GeoDataCreator* creator);
	~GeoDataPolygonGroupCsvImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	CoordinateSystemConverter* m_converter;
};

#endif // GEODATAPOLYGONGROUPCSVIMPORTER_H
