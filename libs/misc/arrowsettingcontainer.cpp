#include "arrowsettingcontainer.h"

const int ArrowSettingContainer::DEFAULT_SAMPLING_RATE = 2;
const int ArrowSettingContainer::DEFAULT_SAMPLING_NUMBER = 100;
const double ArrowSettingContainer::DEFAULT_LEGEND_STANDARD = 1;
const int ArrowSettingContainer::DEFAULT_LEGEND_LENGTH = 100;
const double ArrowSettingContainer::DEFAULT_LEGEND_MINIMUM = 0.001;

ArrowSettingContainer::ArrowSettingContainer() :
	CompositeContainer({&target, &samplingMode, &samplingRate, samplingNumber,
			&colorMode, &customColor, &colorTarget,
			&lengthMode, &standardValue, &legendLength, &minimumValue}),
	target {"attribute"},
	samplingMode {"samplingMode", SamplingMode::All},
	samplingRate {"samplingRate", DEFAULT_SAMPLING_NUMBER},
	samplingNumber {"samplingNumber", DEFAULT_SAMPLING_NUMBER},
	colorMode {"colorMode", ColorMode::Custom},
	customColor {"customColor", Qt::black},
	colorTarget {"colorAttribute"},
	lengthMode {"lengthMode", LengthMode::Auto},
	standardValue {"standardValue", DEFAULT_LEGEND_STANDARD},
	legendLength {"legendLength", DEFAULT_LEGEND_LENGTH},
	minimumValue {"minimumValue", DEFAULT_LEGEND_MINIMUM}
{}

ArrowSettingContainer::ArrowSettingContainer(const ArrowSettingContainer& c) :
	ArrowSettingContainer()
{
	copyValue(c);
}

ArrowSettingContainer& ArrowSettingContainer::operator=(const ArrowSettingContainer& c)
{
	copyValue(c);

	return *this;
}
