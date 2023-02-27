#ifndef POLYDATASETTING_H
#define POLYDATASETTING_H

#include "../postbase_global.h"

#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class POSTBASEDLL_EXPORT PolyDataSetting : public CompositeContainer
{
public:
	enum class Mapping {
		Value,
		Arbitrary
	};

	PolyDataSetting();
	PolyDataSetting(const PolyDataSetting& setting);

	PolyDataSetting& operator=(const PolyDataSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<Mapping> mapping;
	ColorContainer color;
	StringContainer value;
	OpacityContainer opacity;
	IntContainer lineWidth;
};

#endif // POLYDATASETTING_H
