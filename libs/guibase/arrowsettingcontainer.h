#ifndef ARROWSETTINGCONTAINER_H
#define ARROWSETTINGCONTAINER_H

#include "guibase_global.h"
#include "scalarbarsetting.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>

#include <QColor>

class vtkPointSet;
class vtkPolyData;

/// Container for arrow settings
class GUIBASEDLL_EXPORT ArrowSettingContainer : public CompositeContainer
{
public:
	const static int DEFAULT_SAMPLING_RATE;
	const static int DEFAULT_SAMPLING_NUMBER;

	const static double DEFAULT_LEGEND_STANDARD;
	const static int DEFAULT_LEGEND_LENGTH;
	const static double DEFAULT_LEGEND_MINIMUM;

	static const int DEFAULT_ARROWSIZE;
	static const int DEFAULT_LINEWIDTH;

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
	virtual ~ArrowSettingContainer();

	ArrowSettingContainer& operator=(const ArrowSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	double scaleFactor(double stdDistance) const;
	vtkPolyData* buildFilteredData(vtkPointSet* data) const;
	void updateStandardValueIfNeeded(vtkPointSet* data);

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

	IntContainer arrowSize;
	IntContainer lineWidth;

	ScalarBarSetting scalarBarSetting;

	DoubleContainer oldCameraScale;
};

#endif // ARROWSETTINGCONTAINER_H
