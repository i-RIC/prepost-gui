#ifndef GRIDBIRDEYEWINDOWSETTING_H
#define GRIDBIRDEYEWINDOWSETTING_H

#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/doublecontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/stringcontainer.h>

class GridBirdEyeWindowSetting : public CompositeContainer
{
public:
	enum Mapping {Value, Arbitrary};

	GridBirdEyeWindowSetting();
	GridBirdEyeWindowSetting(const GridBirdEyeWindowSetting& setting);

	GridBirdEyeWindowSetting& operator=(const GridBirdEyeWindowSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	StringContainer warpTarget;

	EnumContainerT<Mapping> mapping;
	StringContainer colorTarget;
	ColorContainer customColor;

	BoolContainer showAxes;
	ColorContainer axesColor;

	DoubleContainer zScale;

	ColorContainer backgroundColor;
};

#endif // GRIDBIRDEYEWINDOWSETTING_H
