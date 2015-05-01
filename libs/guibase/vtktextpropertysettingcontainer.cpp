#include "vtktextpropertysettingcontainer.h"

#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <vtkTextProperty.h>

vtkTextPropertySettingContainer::vtkTextPropertySettingContainer()
{
	m_fontFamily = ffArial;
	m_fontColor = QColor(Qt::black);
	m_fontSize = 10;
	m_isBold = false;
	m_isItalic = false;
	m_isShadow = false;
}

void vtkTextPropertySettingContainer::load(const QDomNode& node)
{
	m_fontFamily = static_cast<FontFamily>(iRIC::getIntAttribute(node, attName("fontFamily"), 0));
	m_fontColor = iRIC::getColorAttribute(node, attName("fontColor"));
	m_fontSize = iRIC::getIntAttribute(node, "fontSize", 10);
	m_isBold = iRIC::getBooleanAttribute(node, attName("fontIsBold"));
	m_isItalic = iRIC::getBooleanAttribute(node, attName("fontIsItalic"));
	m_isShadow = iRIC::getBooleanAttribute(node, attName("fontIsShadow"));
}

void vtkTextPropertySettingContainer::save(QXmlStreamWriter& writer) const
{
	iRIC::setIntAttribute(writer, attName("fontFamily"), static_cast<int>(m_fontFamily));
	iRIC::setColorAttribute(writer, attName("fontColor"), m_fontColor);
	iRIC::setIntAttribute(writer, attName("fontSize"), m_fontSize);
	iRIC::setBooleanAttribute(writer, attName("fontIsBold"), m_isBold);
	iRIC::setBooleanAttribute(writer, attName("fontIsItalic"), m_isItalic);
	iRIC::setBooleanAttribute(writer, attName("fontIsShadow"), m_isShadow);
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
	m_fontColor.setRedF(r);
	m_fontColor.setGreenF(g);
	m_fontColor.setBlueF(b);
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
	prop->SetColor(m_fontColor.redF(), m_fontColor.greenF(), m_fontColor.blueF());
	prop->SetFontSize(m_fontSize);
	prop->SetBold(m_isBold);
	prop->SetItalic(m_isItalic);
	prop->SetShadow(m_isShadow);
}

