#include "hydraulicdatariversurveywaterelevation.h"
#include "hydraulicdatariversurveywaterelevationcreator.h"
#include "hydraulicdatariversurveywaterelevationimporter.h"

HydraulicDataRiverSurveyWaterElevationCreator::HydraulicDataRiverSurveyWaterElevationCreator() :
	HydraulicDataCreator ("waterelevation", tr("Water Elevation"))
{
	importers().push_back(new HydraulicDataRiverSurveyWaterElevationImporter(this));
}

QString HydraulicDataRiverSurveyWaterElevationCreator::createName(unsigned int index) const
{
	return QString("wse%1").arg(index);
}

QString HydraulicDataRiverSurveyWaterElevationCreator::createCaption(unsigned int index) const
{
	return QString("WSE%1").arg(index);
}

HydraulicData* HydraulicDataRiverSurveyWaterElevationCreator::create(ProjectDataItem* parent)
{
	return new HydraulicDataRiverSurveyWaterElevation(parent);
}
