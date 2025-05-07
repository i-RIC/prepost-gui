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
	GeoDataImporterSetting* createSetting() const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	CoordinateSystemConverter* m_converter;

	class ImporterSetting;
};

#endif // GEODATAPOLYLINEGROUPCSVIMPORTER_H
