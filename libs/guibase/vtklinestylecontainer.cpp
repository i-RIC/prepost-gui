#include "vtklinestylecontainer.h"

#include <QColor>

#include <vtkProperty.h>

vtkLineStyleContainer::vtkLineStyleContainer() :
	CompositeContainer({&color, &width, &linePattern}),
	color {"color", Qt::black},
	width {"width", 1},
	linePattern{"pattern", 0xffff}
{}

vtkLineStyleContainer::vtkLineStyleContainer(const vtkLineStyleContainer& c) :
	vtkLineStyleContainer {}
{
	CompositeContainer::copyValue(c);
}

vtkLineStyleContainer::~vtkLineStyleContainer()
{}

vtkLineStyleContainer& vtkLineStyleContainer::operator=(const vtkLineStyleContainer& c)
{
	CompositeContainer::copyValue(c);
	return *this;
}

XmlAttributeContainer& vtkLineStyleContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const vtkLineStyleContainer&>(c));
}

void vtkLineStyleContainer::getSetting(vtkProperty* prop)
{
	QColor newColor;
	double r, g, b;

	prop->GetColor(r, g, b);
	newColor.setRedF(r);
	newColor.setGreenF(g);
	newColor.setBlueF(b);

	color.setValue(newColor);
	width = static_cast<int>(prop->GetLineWidth());
	linePattern = prop->GetLineStipplePattern();
}

void vtkLineStyleContainer::applySetting(vtkProperty* prop)
{
	prop->SetColor(color);
	prop->SetLineWidth(width);
	prop->SetLineStipplePattern(linePattern);
}
