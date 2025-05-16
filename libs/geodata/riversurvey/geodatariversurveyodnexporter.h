#ifndef GEODATARIVERSURVEYODNEXPORTER_H
#define GEODATARIVERSURVEYODNEXPORTER_H

#include <guicore/pre/geodata/geodataexporter.h>

class GeoDataRiverSurvey;

class GeoDataRiverSurveyOdnExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyOdnExporter(GeoDataCreator* creator);
	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

private:
	bool check(GeoDataRiverSurvey* rs, QWidget* w);

	struct Problem {
		QString name;
		QString message;
	};

	class ProblemsDialog;
};

#endif // GEODATARIVERSURVEYODNEXPORTER_H
