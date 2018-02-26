#ifndef GEODATARIVERSURVEYIMPORTER_H
#define GEODATARIVERSURVEYIMPORTER_H

#include "geodatariversurveyimportersettingdialog.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <vector>

namespace {
class RivPathPoint;
}

class GeoDataRiverSurveyImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataRiverSurveyImporter(GeoDataCreator* creator);

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;
	void clearPoints();

	std::vector<RivPathPoint*> m_points;
	bool m_with4Points;
	bool m_allNamesAreNumber;
	bool m_reverseOrder;
	GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting m_cpSetting;
};

#endif // GEODATARIVERSURVEYIMPORTER_H
