#ifndef VTKTEXTPROPERTYSETTINGCONTAINER_H
#define VTKTEXTPROPERTYSETTINGCONTAINER_H

#include "guibase_global.h"
#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/compositecontainer.h>

class QColor;
class vtkTextProperty;

/// Container class for vtkTextProperty setting
class GUIBASEDLL_EXPORT vtkTextPropertySettingContainer : public CompositeContainer
{
public:
	enum FontFamily {
		ffArial   = 0,   ///< Arial
		ffCourier = 1,   ///< Courier
		ffTimes   = 2    ///< Times
	};
	vtkTextPropertySettingContainer();
	vtkTextPropertySettingContainer(const vtkTextPropertySettingContainer& c);
	~vtkTextPropertySettingContainer();

	vtkTextPropertySettingContainer& operator=(const vtkTextPropertySettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void getSetting(vtkTextProperty* prop);
	void applySetting(vtkTextProperty* prop);

public:
	EnumContainerT<FontFamily> fontFamily;
	ColorContainer fontColor;
	IntContainer fontSize;
	BoolContainer isBold;
	BoolContainer isItalic;
	BoolContainer isShadow;
};

#endif // VTKTEXTPROPERTYSETTINGCONTAINER_H
