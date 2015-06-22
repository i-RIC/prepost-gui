#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATIONIMPORTER_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATIONIMPORTER_H

#include "hd_riversurveywaterelevation_global.h"
#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>

class HD_RIVERSURVEYWATERELEVATION_EXPORT HydraulicDataRiverSurveyWaterElevationImporter : public HydraulicDataImporter
{
	Q_OBJECT

public:
	/// Constructor
	HydraulicDataRiverSurveyWaterElevationImporter();
	/// Import hydraulicdata from the specified file.
	bool import(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w) override;
	/// Returns true if the hydraulic data can be imported to the specified geodata.
	bool canImportTo(GeoData* data) override;
	const QStringList fileDialogFilters() override;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATIONIMPORTER_H
