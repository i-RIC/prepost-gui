#ifndef GEODATARIVERSURVEYODNIMPORTER_H
#define GEODATARIVERSURVEYODNIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataRiverSurveyOdnImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyOdnImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

	bool importData(GeoData* data, int index, QWidget* w) override;
};

#endif // GEODATARIVERSURVEYODNIMPORTER_H
