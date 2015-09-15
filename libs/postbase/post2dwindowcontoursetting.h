#ifndef POST2DWINDOWCONTOURSETTING_H
#define POST2DWINDOWCONTOURSETTING_H

#include "postbase_global.h"

#include <guibase/structuredgridregion.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/boolcontainer.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/enumcontainert.h>
#include <guibase/contoursettingwidget.h>

class POSTBASEDLL_EXPORT Post2dWindowContourSetting : public CompositeContainer
{

public:
	const static int DEFAULT_NUMOFDIV;
	/// Constructor
	Post2dWindowContourSetting();

	IntContainer numberOfDivisions;
	StringContainer currentSolution;
	EnumContainerT<ContourSettingWidget::Contour> contour;
	BoolContainer fillUpper;
	BoolContainer fillLower;
	OpacityContainer opacity;

	// Region setting
	EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	StructuredGridRegion::Range2d range;

	// for scalar bar
	ScalarBarSetting scalarBarSetting;
	vtkTextPropertySettingContainer titleTextSetting;
	vtkTextPropertySettingContainer labelTextSetting;
};


#endif // POST2DWINDOWCONTOURSETTING_H
