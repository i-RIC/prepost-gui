#ifndef GEODATARIVERSURVEYLANDXMLEXPORTER_H
#define GEODATARIVERSURVEYLANDXMLEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataRiverSurvey;
class QXmlStreamWriter;

class GeoDataRiverSurveyLandXmlExporter : public GeoDataExporter
{
public:
	GeoDataRiverSurveyLandXmlExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

private:
	static void writeAlignmentsForLeftDistanceMarkAndCrossSection(QXmlStreamWriter* writer, GeoDataRiverSurvey* rs);
	static void writeAlignmentsForRightDistanceMark(QXmlStreamWriter* writer, GeoDataRiverSurvey* rs);
};

#endif // GEODATARIVERSURVEYLANDXMLEXPORTER_H
