#ifndef ARROWSSETTINGCONTAINER_H
#define ARROWSSETTINGCONTAINER_H

#include "../guicore_global.h"
#include "../image/imagesettingcontainer.h"
#include "arrowslegendsettingcontainer.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/qfontcontainer.h>
#include <misc/stringcontainer.h>

class VTKGraphicsView;

class vtkConeSource;
class vtkDataSetAttributes;
class vtkPointSet;
class vtkPolyData;

class GUICOREDLL_EXPORT ArrowsSettingContainer : public CompositeContainer
{
public:
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

	ArrowsSettingContainer();
	ArrowsSettingContainer(const ArrowsSettingContainer& c);
	virtual ~ArrowsSettingContainer();

	ArrowsSettingContainer& operator=(const ArrowsSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	double scaleFactor(double stdDistance) const;
	vtkPolyData* buildFilteredData(vtkPointSet* data) const;
	vtkPolyData* buildArrowsPolygonData(vtkPointSet* data, VTKGraphicsView* view, double zscale = 1.0) const;
	void updateStandardValueIfNeeded(vtkDataSetAttributes* atts);

	StringContainer target;

	EnumContainerT<ColorMode> colorMode;
	ColorContainer customColor;
	StringContainer colorTarget;

	EnumContainerT<LengthMode> lengthMode;
	DoubleContainer standardValue;
	DoubleContainer legendLength;
	DoubleContainer minimumValue;

	IntContainer arrowSize;
	IntContainer lineWidth;

	ArrowsLegendSettingContainer legend;

private:
	vtkConeSource* buildConeSource(VTKGraphicsView* view) const;
};

#endif // ARROWSSETTINGCONTAINER_H
