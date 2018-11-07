#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATIONIMPORTER_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATIONIMPORTER_H

#include "hd_riversurveywaterelevation_global.h"
#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>

class HD_RIVERSURVEYWATERELEVATION_EXPORT HydraulicDataRiverSurveyWaterElevationImporter : public HydraulicDataImporter
{
	Q_OBJECT

public:
	HydraulicDataRiverSurveyWaterElevationImporter(HydraulicDataCreator* creator);

	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;

	bool importData(HydraulicData* data, int index, const std::set<QString>& usedCaptions, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, QWidget* w) override;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATIONIMPORTER_H
