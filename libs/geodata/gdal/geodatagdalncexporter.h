#ifndef GEODATANETCDFNCEXPORTER_H
#define GEODATANETCDFNCEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataNetcdfNcExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataNetcdfNcExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATANETCDFNCEXPORTER_H
