#ifndef SCALARBARSETTING_H
#define SCALARBARSETTING_H

#include "guibase_global.h"
#include <misc/boolcontainer.h>
#include <misc/intcontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class vtkScalarBarRepresentation;
class vtkLegendBoxRepresentation;

#include <QString>

/// Container for scalar bar display setting
class GUIBASEDLL_EXPORT ScalarBarSetting : public CompositeContainer
{

public:
	static const int DEFAULT_NUMOFLABELS;
	static const QString DEFAULT_LABELFORMAT;
	enum class Orientation {Vertical, Horizontal};

	/// Constructor
	ScalarBarSetting();
	/// Constructor (copy)
	ScalarBarSetting(const ScalarBarSetting& setting);
	/// Destructor
	~ScalarBarSetting() {}
	void initForLegendBox();
	/// Load setting from vtkScalarBarRepresentation
	void loadFromRepresentation(vtkScalarBarRepresentation* rep);
	/// Load setting from vtkLegendBoxRepresentation
	void loadFromRepresentation(vtkLegendBoxRepresentation* rep);
	/// Apply setting to vtkScalarBarRepresentation
	void saveToRepresentation(vtkScalarBarRepresentation* rep) const;
	/// Apply setting to vtkLegendBoxRepresentation
	void saveToRepresentation(vtkLegendBoxRepresentation* rep);

	BoolContainer visible;
	EnumContainerT<Orientation> orientation;
	IntContainer numberOfLabels;
	/// relative width (0 < width < 1)
	DoubleContainer width;
	/// relative width (0 < height < 1)
	DoubleContainer height;
	/// relative position (0 < positionX < 1)
	DoubleContainer positionX;
	/// relative position (0 < positionY < 1)
	DoubleContainer positionY;
	/// labelFormat
	StringContainer labelFormat;
};

#endif // SCALARBARSETTING_H
