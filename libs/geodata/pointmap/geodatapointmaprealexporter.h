#ifndef GEODATAPOINTMAPREALEXPORTER_H
#define GEODATAPOINTMAPREALEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataPointmapRealExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPointmapRealExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOINTMAPREALEXPORTER_H
