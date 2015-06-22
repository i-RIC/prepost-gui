#ifndef GEODATAPOINTMAPVTKEXPORTER_H
#define GEODATAPOINTMAPVTKEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataPointmapVTKExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPointmapVTKExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOINTMAPVTKEXPORTER_H
