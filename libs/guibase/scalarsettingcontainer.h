#ifndef SCALARSETTINGCONTAINER_H
#define SCALARSETTINGCONTAINER_H

#include "guibase_global.h"

#include "scalarbarsetting.h"
#include "vtktextpropertysettingcontainer.h"

#include <guibase/widget/contoursettingwidget.h>
#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class LookupTableContainer;
class vtkPointSet;
class vtkUnstructuredGrid;

class GUIBASEDLL_EXPORT ScalarSettingContainer : public CompositeContainer
{
public:
	const static int DEFAULT_NUMOFDIV;

	ScalarSettingContainer();
	ScalarSettingContainer(const ScalarSettingContainer& c);
	virtual ~ScalarSettingContainer();

	ScalarSettingContainer& operator=(const ScalarSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	vtkPointSet* filterCellsWithUpperLower(vtkPointSet* ps, const LookupTableContainer& lut) const;

	StringContainer target;

	EnumContainerT<ContourSettingWidget::Contour> contour;
	IntContainer numberOfDivisions;
	BoolContainer fillUpper;
	BoolContainer fillLower;
	OpacityContainer opacity;

	ScalarBarSetting scalarBarSetting;
};

#endif // SCALARSETTINGCONTAINER_H
