#include "distancemeasuresetting.h"

#include <QColor>

DistanceMeasureSetting::DistanceMeasureSetting() :
	CompositeContainer({&defined, &point1, &point2,
		&showLabel, &labelMode, &labelPosition, &labelFontSize, &customLabel,
		&showMarkers, &markerSize, &color}),
	defined {"defined", false},
	point1 {"point1"},
	point2 {"point2"},
	showLabel {"showLabel", true},
	labelMode {"labelMode", Auto},
	labelPosition {"labelPosition", vtkLabel2DActor::lpTopCenter},
	labelFontSize {"fontSize", 12},
	customLabel {"customLabel"},
	showMarkers {"showMarkers", true},
	markerSize {"markerSize", 7},
	color {"color", Qt::black}
{}

DistanceMeasureSetting::DistanceMeasureSetting(const DistanceMeasureSetting& setting) :
	DistanceMeasureSetting {}
{
	CompositeContainer::copyValue(setting);
}

DistanceMeasureSetting::~DistanceMeasureSetting()
{}

DistanceMeasureSetting& DistanceMeasureSetting::operator=(const DistanceMeasureSetting& setting)
{
	CompositeContainer::copyValue(setting);
	return *this;
}

XmlAttributeContainer& DistanceMeasureSetting::operator=(const XmlAttributeContainer& c)
{
	return operator =(dynamic_cast<const DistanceMeasureSetting&> (c));
}
