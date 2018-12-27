#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATIONCREATOR_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATIONCREATOR_H

#include "hd_riversurveywaterelevation_global.h"

#include <guicore/pre/hydraulicdata/hydraulicdatacreator.h>

class HD_RIVERSURVEYWATERELEVATION_EXPORT HydraulicDataRiverSurveyWaterElevationCreator : public HydraulicDataCreator
{
	Q_OBJECT

public:
	HydraulicDataRiverSurveyWaterElevationCreator();

	QString createName(unsigned int index) const override;
	QString createCaption(unsigned int index) const override;

	HydraulicData* create(ProjectDataItem* parent) override;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATIONCREATOR_H
