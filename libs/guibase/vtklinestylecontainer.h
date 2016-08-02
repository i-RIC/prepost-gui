#ifndef VTKLINESTYLECONTAINER_H
#define VTKLINESTYLECONTAINER_H

#include "guibase_global.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>

class vtkProperty;

class GUIBASEDLL_EXPORT vtkLineStyleContainer : public CompositeContainer
{
public:
	enum LinePattern {
		Solid = 0xffff,
		Dashed = 0xf0f0,
		Dotted = 0xcccc,
	};

	vtkLineStyleContainer();
	vtkLineStyleContainer(const vtkLineStyleContainer& c);
	~vtkLineStyleContainer();

	vtkLineStyleContainer& operator=(const vtkLineStyleContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	ColorContainer color;
	IntContainer width;
	IntContainer linePattern;

	void getSetting(vtkProperty* prop);
	void applySetting(vtkProperty* prop);
};

#endif // VTKLINESTYLECONTAINER_H
