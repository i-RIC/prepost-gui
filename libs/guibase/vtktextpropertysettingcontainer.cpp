#include "vtktextpropertysettingcontainer.h"

#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkTextProperty.h>

vtkTextPropertySettingContainer::vtkTextPropertySettingContainer() :
	CompositeContainer {&m_fontFamily, &m_fontColor, &m_fontSize, &m_isBold, &m_isItalic, &m_isShadow},
	m_fontFamily {"fontFamily", ffArial},
	m_fontColor {"fontColor", Qt::black},
	m_fontSize {"fontSize", 10},
	m_isBold {"fontIsBold", false},
	m_isItalic {"fontIsItalic", false},
	m_isShadow {"fontIsShadow", false}
{}

vtkTextPropertySettingContainer::vtkTextPropertySettingContainer(const vtkTextPropertySettingContainer &c) :
	vtkTextPropertySettingContainer {}
{
	*this = c;
}

void vtkTextPropertySettingContainer::getSetting(vtkTextProperty* prop)
{
	if (prop->GetFontFamily() == VTK_ARIAL) {
		m_fontFamily = ffArial;
	} else if (prop->GetFontFamily() == VTK_COURIER) {
		m_fontFamily = ffCourier;
	} else if (prop->GetFontFamily() == VTK_TIMES) {
		m_fontFamily = ffTimes;
	}
	double r, g, b;
	prop->GetColor(r, g, b);
	QColor c;
	c.setRedF(r);
	c.setGreenF(g);
	c.setBlueF(b);
	m_fontColor = c;
	m_isBold = (prop->GetBold() == 1);
	m_isItalic = (prop->GetItalic() == 1);
	m_isShadow = (prop->GetShadow() == 1);
}

void vtkTextPropertySettingContainer::applySetting(vtkTextProperty* prop)
{
	if (m_fontFamily == ffArial) {
		prop->SetFontFamilyToArial();
	} else if (m_fontFamily == ffCourier) {
		prop->SetFontFamilyToCourier();
	} else if (m_fontFamily == ffTimes) {
		prop->SetFontFamilyToTimes();
	}
	prop->SetColor(m_fontColor);
	prop->SetFontSize(m_fontSize);
	prop->SetBold(m_isBold);
	prop->SetItalic(m_isItalic);
	prop->SetShadow(m_isShadow);
}
