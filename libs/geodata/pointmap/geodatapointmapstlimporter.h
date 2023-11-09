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

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool checkHeader(const QString& filename, QWidget* w);

	CoordinateSystemConverter* m_converter;
};

#endif // GEODATAPOINTMAPSTLIMPORTER_H
