#ifndef GEODATARIVERSURVEY_DISPLAYSETTING_H
#define GEODATARIVERSURVEY_DISPLAYSETTING_H

#include "../geodatariversurvey.h"

#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>

class GeoDataRiverSurvey::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& setting);

	DisplaySetting& operator=(const DisplaySetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer showBackground;
	BoolContainer showLines;
	OpacityContainer opacity;
	IntContainer crosssectionLinesScale;
	ColorContainer crosssectionLinesColor;
	BoolContainer showNames;
	vtkTextPropertySettingContainer namesTextSetting;
};

#endif // GEODATARIVERSURVEY_DISPLAYSETTING_H
