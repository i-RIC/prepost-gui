#ifndef GEODATARIVERSURVEYVTKEXPORTER_H
#define GEODATARIVERSURVEYVTKEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataRiverSurveyVTKExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyVTKExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // GEODATARIVERSURVEYVTKEXPORTER_H
