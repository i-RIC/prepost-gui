#ifndef CONTOURSETTINGCONTAINER_H
#define CONTOURSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>

class ColorMapSettingContainer;

class vtkContourFilter;
class vtkPointSet;

class GUICOREDLL_EXPORT ContourSettingContainer : public CompositeContainer
{
public:
	enum class Mode {
		UseColorMapValues,
		SpecifyNumberOfValues
	};

	ContourSettingContainer();
	ContourSettingContainer(const ContourSettingContainer& c);
	~ContourSettingContainer();

	ContourSettingContainer& operator=(const ContourSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void setColorMapSetting(ColorMapSettingContainer* setting);

	vtkPointSet* buildFilteredData(vtkPointSet* data) const;

	BoolContainer drawContourLines;
	IntContainer contourLineWidth;
	EnumContainerT<Mode> mode;
	IntContainer numberOfValues;

private:
	ColorMapSettingContainer* m_colorMapSetting;
	vtkContourFilter* m_filter;
};

#endif // CONTOURSETTINGCONTAINER_H
