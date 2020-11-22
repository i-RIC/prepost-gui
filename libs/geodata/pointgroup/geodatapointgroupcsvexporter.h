#ifndef GEODATAPOINTGROUPCSVEXPORTER_H
#define GEODATAPOINTGROUPCSVEXPORTER_H

#include "gd_pointgroup_global.h"

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataPointGroupCsvExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPointGroupCsvExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOINTGROUPCSVEXPORTER_H
