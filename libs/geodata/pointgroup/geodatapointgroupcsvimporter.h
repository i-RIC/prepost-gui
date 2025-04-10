#ifndef GEODATAPOINTGROUPCSVIMPORTER_H
#define GEODATAPOINTGROUPCSVIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

class CoordinateSystemConverter;

class GeoDataPointGroupCsvImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointGroupCsvImporter(GeoDataCreator* creator);
	~GeoDataPointGroupCsvImporter();

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


#endif // GEODATAPOINTGROUPCSVIMPORTER_H
