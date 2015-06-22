#ifndef GEODATAPOINTMAPSTLEXPORTER_H
#define GEODATAPOINTMAPSTLEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataPointmapSTLExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPointmapSTLExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOINTMAPSTLEXPORTER_H
