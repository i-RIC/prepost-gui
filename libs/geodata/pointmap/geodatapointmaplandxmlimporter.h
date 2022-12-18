#ifndef GEODATAPOINTMAPLANDXMLIMPORTER_H
#define GEODATAPOINTMAPLANDXMLIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystemConverter;

class GeoDataPointmapLandXmlImporter : public GeoDataImporter
{
public:
	GeoDataPointmapLandXmlImporter(GeoDataCreator* creator);
	~GeoDataPointmapLandXmlImporter();

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	bool importDataDOM(GeoData* data, int index, QWidget* w);
	bool importDataSAX(GeoData* data, int index, QWidget* w);

	CoordinateSystemConverter* m_converter;
};

#endif // GEODATAPOINTMAPLANDXMLIMPORTER_H
