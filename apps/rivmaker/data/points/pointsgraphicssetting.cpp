#include "pointsgraphicssetting.h"

PointsGraphicsSetting::PointsGraphicsSetting() :
	sizeSetting {SizeSetting::Auto},
	manualSize {5},
	transparency {0}
{}

int PointsGraphicsSetting::size() const
{
	if (sizeSetting == PointsGraphicsSetting::SizeSetting::Auto) {
		return autoSize;
	} else {
		return manualSize;
	}
}

PointsGraphicsSetting PointsGraphicsSetting::elevationPointsSetting;
PointsGraphicsSetting PointsGraphicsSetting::waterElevationPointsSetting;
