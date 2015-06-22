#ifndef GEODATARIVERSURVEYEXPORTER_H
#define GEODATARIVERSURVEYEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataRiverSurveyExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATAPOINTMAPREALEXPORTER_H
