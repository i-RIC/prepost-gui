#ifndef GEODATARIVERSURVEYHONRYUEXPORTER_H
#define GEODATARIVERSURVEYHONRYUEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataRiverSurveyHonryuExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyHonryuExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATARIVERSURVEYHONRYUEXPORTER_H
