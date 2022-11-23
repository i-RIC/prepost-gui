#ifndef GEODATARIVERSURVEYDISPLAYSETTING_H
#define GEODATARIVERSURVEYDISPLAYSETTING_H

#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>

class GeoDataRiverSurveyDisplaySetting : public CompositeContainer
{
public:
	GeoDataRiverSurveyDisplaySetting();
	GeoDataRiverSurveyDisplaySetting(const GeoDataRiverSurveyDisplaySetting& setting);

	GeoDataRiverSurveyDisplaySetting& operator=(const GeoDataRiverSurveyDisplaySetting& setting);

	BoolContainer showBackground;
	BoolContainer showLines;
	OpacityContainer opacity;
	IntContainer crosssectionLinesScale;
	ColorContainer crosssectionLinesColor;
	BoolContainer showNames;
	vtkTextPropertySettingContainer namesTextSetting;
};

#endif // GEODATARIVERSURVEYDISPLAYSETTING_H
