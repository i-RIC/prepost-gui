#ifndef GEODATAPOINTMAPRITER3DIMPORTER_H
#define GEODATAPOINTMAPRITER3DIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataPointmapRiter3dLoader;

class GeoDataPointmapRiter3dImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointmapRiter3dImporter(GeoDataCreator* creator);
	~GeoDataPointmapRiter3dImporter();

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	GeoDataPointmapRiter3dLoader* m_loader;
	QString m_fileName;
};

#endif // GEODATAPOINTMAPRITER3DIMPORTER_H
