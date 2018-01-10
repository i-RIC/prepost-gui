#include "arrowsettingcontainer.h"

#include <QSettings>

const int ArrowSettingContainer::DEFAULT_SAMPLING_RATE = 1;
const int ArrowSettingContainer::DEFAULT_SAMPLING_NUMBER = 100;

const double ArrowSettingContainer::DEFAULT_LEGEND_STANDARD = 1;
const int ArrowSettingContainer::DEFAULT_LEGEND_LENGTH = 100;
const double ArrowSettingContainer::DEFAULT_LEGEND_MINIMUM = 0.001;

const int ArrowSettingContainer::DEFAULT_ARROWSIZE = 8;
const int ArrowSettingContainer::DEFAULT_LINEWIDTH = 1;

ArrowSettingContainer::ArrowSettingContainer() :
	CompositeContainer({&target, &samplingMode, &samplingRate, &samplingNumber,
			&colorMode, &customColor, &colorTarget,
			&lengthMode, &standardValue, &legendLength, &minimumValue,
			&arrowSize, &lineWidth}),
	target {"attribute"},
	samplingMode {"samplingMode", SamplingMode::All},
	samplingRate {"samplingRate", DEFAULT_SAMPLING_RATE},
	samplingNumber {"samplingNumber", DEFAULT_SAMPLING_NUMBER},
	colorMode {"colorMode", ColorMode::Custom},
	customColor {"customColor", Qt::black},
	colorTarget {"colorAttribute"},
	lengthMode {"lengthMode", LengthMode::Auto},
	standardValue {"standardValue", DEFAULT_LEGEND_STANDARD},
	legendLength {"legendLength", DEFAULT_LEGEND_LENGTH},
	minimumValue {"minimumValue", DEFAULT_LEGEND_MINIMUM},
	arrowSize {"arrowSize", DEFAULT_ARROWSIZE},
	lineWidth {"arrowLineWidth", DEFAULT_LINEWIDTH},
	oldCameraScale {"oldCameraScale", 1}
{
	QSettings setting;
	customColor = setting.value("graphics/vectorcolor", QColor(Qt::black)).value<QColor>();
}

ArrowSettingContainer::ArrowSettingContainer(const ArrowSettingContainer& c) :
	ArrowSettingContainer()
{
	copyValue(c);
}

ArrowSettingContainer::~ArrowSettingContainer()
{}

ArrowSettingContainer& ArrowSettingContainer::operator=(const ArrowSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ArrowSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ArrowSettingContainer&> (c));
}
