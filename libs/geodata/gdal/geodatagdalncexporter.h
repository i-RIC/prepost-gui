#ifndef GEODATAGDALNCEXPORTER_H
#define GEODATAGDALNCEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataGdalNcExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataGdalNcExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAGDALNCEXPORTER_H
