#ifndef GEODATAPOLYGONGROUPCSVIMPORTER_H
#define GEODATAPOLYGONGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystemConverter;
class GeoDataPolygonGroup;

class GeoDataPolygonGroupCsvImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPolygonGroupCsvImporter(GeoDataCreator* creator);
	~GeoDataPolygonGroupCsvImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	GeoDataImporterSetting* createSetting() const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	static void addNewPolygon(GeoDataPolygonGroup* group, const QString& name, double value, QPolygonF polygon);

	CoordinateSystemConverter* m_converter;

	class ImporterSetting;
};

#endif // GEODATAPOLYGONGROUPCSVIMPORTER_H
