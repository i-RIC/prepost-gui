#ifndef GEODATAPOLYLINEGROUPCSVIMPORTER_H
#define GEODATAPOLYLINEGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystemConverter;

class GeoDataPolyLineGroupCsvImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPolyLineGroupCsvImporter(GeoDataCreator* creator);
	~GeoDataPolyLineGroupCsvImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	CoordinateSystemConverter* m_converter;
};

#endif // GEODATAPOLYLINEGROUPCSVIMPORTER_H
