#ifndef GEODATARIVERSURVEYMLITIMPORTER_H
#define GEODATARIVERSURVEYMLITIMPORTER_H

#include "geodatariversurveyimporter.h"
#include "geodatariversurveyimportersettingdialog.h"

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataRiverSurveyMlitImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyMlitImporter(GeoDataCreator* creator);

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	GeoDataImporterSetting* createSetting() const override;

	bool importInit(const QString& filename, const QString& csFolder, QWidget* w);

private:
	bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;
	bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	std::vector<GeoDataRiverSurveyImporter::RivPathPoint*> m_points;
	bool m_with4Points;
	GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting m_cpSetting;
	QString m_csvFilename;

public:
	class ImporterSetting;
	class ProblemsDialog;
};

#endif // GEODATARIVERSURVEYMLITIMPORTER_H
