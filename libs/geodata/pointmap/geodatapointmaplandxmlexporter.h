#ifndef GEODATAPOINTMAPLANDXMLEXPORTER_H
#define GEODATAPOINTMAPLANDXMLEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataPointmapLandXmlExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPointmapLandXmlExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOINTMAPLANDXMLEXPORTER_H
