#ifndef GEODATAPOINTMAPSTLIMPORTER_H
#define GEODATAPOINTMAPSTLIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystemConverter;

class GeoDataPointmapSTLImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointmapSTLImporter(GeoDataCreator* creator);
	~GeoDataPointmapSTLImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	GeoDataImporterSetting* createSetting() const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool checkHeader(const QString& filename, QWidget* w);

	CoordinateSystemConverter* m_converter;

	class ImporterSetting;
};

#endif // GEODATAPOINTMAPSTLIMPORTER_H
