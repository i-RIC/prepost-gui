#ifndef GEODATAPOINTMAPLANDXMLIMPORTER_H
#define GEODATAPOINTMAPLANDXMLIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystemConverter;

class GeoDataPointmapLandXmlImporter : public GeoDataImporter
{
public:
	GeoDataPointmapLandXmlImporter(GeoDataCreator* creator);
	~GeoDataPointmapLandXmlImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	GeoDataImporterSetting* createSetting() const override;

	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	bool importDataDOM(GeoData* data, int index, QWidget* w);
	bool importDataSAX(GeoData* data, int index, QWidget* w);

	CoordinateSystemConverter* m_converter;

	class ImporterSetting;
};

#endif // GEODATAPOINTMAPLANDXMLIMPORTER_H
