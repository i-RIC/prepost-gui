#ifndef GEODATAPOLYGONGROUPCSVEXPORTER_H
#define GEODATAPOLYGONGROUPCSVEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataPolygonGroupCsvExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPolygonGroupCsvExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOLYGONGROUPCSVEXPORTER_H
