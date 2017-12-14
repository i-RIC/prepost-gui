#ifndef SCALARBARSETTING_H
#define SCALARBARSETTING_H

#include "guibase_global.h"

#include "vtktextpropertysettingcontainer.h"

#include <misc/boolcontainer.h>
#include <misc/intcontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class vtkScalarBarRepresentation;
class vtkLegendBoxRepresentation;

#include <QString>

#include <set>

/// Container for scalar bar display setting
class GUIBASEDLL_EXPORT ScalarBarSetting : public CompositeContainer
{

public:
	static const int DEFAULT_NUMOFLABELS;
	static const QString DEFAULT_LABELFORMAT;
	enum class Orientation {Vertical, Horizontal};
	enum class Quadrant {None, RightLower, LeftLower, LeftUpper, RightUpper};
	static std::set<Quadrant> getQuadrantSet();

	ScalarBarSetting();
	ScalarBarSetting(const ScalarBarSetting& setting);
	~ScalarBarSetting();

	ScalarBarSetting& operator=(const ScalarBarSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting) override;

	void initForLegendBox();

	void setDefaultPosition(Quadrant quadrant);
	double distanceFromDefault(Quadrant quadrant);

	void loadFromRepresentation(vtkScalarBarRepresentation* rep);
	void saveToRepresentation(vtkScalarBarRepresentation* rep) const;

	void loadFromRepresentation(vtkLegendBoxRepresentation* rep);
	void saveToRepresentation(vtkLegendBoxRepresentation* rep);

	BoolContainer visible;

	EnumContainerT<Orientation> orientation;
	EnumContainerT<Quadrant> quadrant;
	IntContainer numberOfLabels;

	DoubleContainer width;      ///< relative width (0 < width < 1)
	DoubleContainer height;     ///< relative width (0 < height < 1)

	DoubleContainer positionX;  ///< relative position (0 < positionX < 1)
	DoubleContainer positionY;  ///< relative position (0 < positionY < 1)

	StringContainer labelFormat;

	vtkTextPropertySettingContainer titleTextSetting;
	vtkTextPropertySettingContainer labelTextSetting;
};

#endif // SCALARBARSETTING_H
