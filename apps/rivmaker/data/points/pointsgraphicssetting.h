#ifndef POINTSGRAPHICSSETTING_H
#define POINTSGRAPHICSSETTING_H

class PointsGraphicsSetting
{
public:
	enum class SizeSetting {
		Auto,
		Manual,
	};
	PointsGraphicsSetting();

	int size() const;

	SizeSetting sizeSetting;
	int autoSize;
	int manualSize;

	int transparency;

	static PointsGraphicsSetting elevationPointsSetting;
	static PointsGraphicsSetting waterElevationPointsSetting;
};

#endif // POINTSGRAPHICSSETTING_H
