#ifndef GEODATARIVERSURVEYPROXY_DISPLAYSETTING_H
#define GEODATARIVERSURVEYPROXY_DISPLAYSETTING_H

#include "geodatariversurvey_displaysetting.h"
#include "../geodatariversurveyproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataRiverSurveyProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataRiverSurvey::DisplaySetting displaySetting;
};

#endif // GEODATARIVERSURVEYPROXY_DISPLAYSETTING_H
