#ifndef ARROWSETTINGCONTAINER_H
#define ARROWSETTINGCONTAINER_H

#include "misc_global.h"
#include "colorcontainer.h"
#include "compositecontainer.h"
#include "doublecontainer.h"
#include "enumcontainert.h"
#include "intcontainer.h"
#include "stringcontainer.h"

#include <QColor>

/// Container for arrow settings
class MISCDLL_EXPORT ArrowSettingContainer : public CompositeContainer
{
public:
	const static int DEFAULT_SAMPLING_RATE;
	const static int DEFAULT_SAMPLING_NUMBER;
	const static double DEFAULT_LEGEND_STANDARD;
	const static int DEFAULT_LEGEND_LENGTH;
	const static double DEFAULT_LEGEND_MINIMUM;

	enum class SamplingMode {
		All  = 1,
		Rate = 2,
		Number = 3,
	};

	enum class ColorMode {
		Custom,
		ByScalar
	};

	enum class LengthMode {
		Auto,
		Custom
	};

	ArrowSettingContainer();
	ArrowSettingContainer(const ArrowSettingContainer& c);

	ArrowSettingContainer& operator=(const ArrowSettingContainer& c);

	StringContainer target;

	EnumContainerT<SamplingMode> samplingMode;
	IntContainer samplingRate;
	IntContainer samplingNumber;

	EnumContainerT<ColorMode> colorMode;

	ColorContainer customColor;
	StringContainer colorTarget;

	EnumContainerT<LengthMode> lengthMode;
	DoubleContainer standardValue;
	DoubleContainer legendLength;
	DoubleContainer minimumValue;
};

#endif // ARROWSETTINGCONTAINER_H
