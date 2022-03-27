#include "vtktextpropertysettingcontainer.h"

#include <misc/xmlsupport.h>

#include <QColor>
#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkTextProperty.h>

vtkTextPropertySettingContainer::vtkTextPropertySettingContainer() :
	CompositeContainer({&fontFamily, &fontColor, &fontSize, &isBold, &isItalic, &isShadow}),
	fontFamily {"fontFamily", ffArial},
	fontColor {"fontColor", Qt::black},
	fontSize {"fontSize", 10},
	isBold {"fontIsBold", false},
	isItalic {"fontIsItalic", false},
	isShadow {"fontIsShadow", false}
{}

vtkTextPropertySettingContainer::vtkTextPropertySettingContainer(const vtkTextPropertySettingContainer& c) :
	vtkTextPropertySettingContainer {}
{
	operator=(c);
}

vtkTextPropertySettingContainer::~vtkTextPropertySettingContainer()
{}

vtkTextPropertySettingContainer& vtkTextPropertySettingContainer::operator=(const vtkTextPropertySettingContainer& c)
{
	return dynamic_cast<vtkTextPropertySettingContainer&>(CompositeContainer::operator=(dynamic_cast<const XmlAttributeContainer&>(c)));
}

XmlAttributeContainer& vtkTextPropertySettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const vtkTextPropertySettingContainer&>(c));
}

void vtkTextPropertySettingContainer::getSetting(vtkTextProperty* prop)
{
	if (prop->GetFontFamily() == VTK_ARIAL) {
		fontFamily = ffArial;
	} else if (prop->GetFontFamily() == VTK_COURIER) {
		fontFamily = ffCourier;
	} else if (prop->GetFontFamily() == VTK_TIMES) {
		fontFamily = ffTimes;
	}
	double r, g, b;
	prop->GetColor(r, g, b);
	QColor c;
	c.setRedF(r);
	c.setGreenF(g);
	c.setBlueF(b);
	fontColor = c;
	isBold = (prop->GetBold() == 1);
	isItalic = (prop->GetItalic() == 1);
	isShadow = (prop->GetShadow() == 1);
}

void vtkTextPropertySettingContainer::applySetting(vtkTextProperty* prop)
{
	if (fontFamily == ffArial) {
		prop->SetFontFamilyToArial();
	} else if (fontFamily == ffCourier) {
		prop->SetFontFamilyToCourier();
	} else if (fontFamily == ffTimes) {
		prop->SetFontFamilyToTimes();
	}
	prop->SetColor(fontColor);
	prop->SetFontSize(fontSize);
	prop->SetBold(isBold);
	prop->SetItalic(isItalic);
	prop->SetShadow(isShadow);
}

