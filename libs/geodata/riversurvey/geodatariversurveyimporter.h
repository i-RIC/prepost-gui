#ifndef GEODATARIVERSURVEYIMPORTER_H
#define GEODATARIVERSURVEYIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>
#include "geodatariversurveyimportersettingdialog.h"

class GeoDataRiverSurveyImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyImporter(GeoDataCreator* creator);

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting m_cpSetting;
};

#endif // GEODATARIVERSURVEYIMPORTER_H
