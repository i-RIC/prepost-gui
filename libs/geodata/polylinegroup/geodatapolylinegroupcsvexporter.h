#ifndef GEODATAPOLYLINEGROUPCSVEXPORTER_H
#define GEODATAPOLYLINEGROUPCSVEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataPolyLineGroupCsvExporter : public GeoDataExporter
{
public:
	GeoDataPolyLineGroupCsvExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOLYLINEGROUPCSVEXPORTER_H
