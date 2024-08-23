#ifndef GRIDATTRIBUTECELLSETTING_H
#define GRIDATTRIBUTECELLSETTING_H

#include "../../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>

class VTKGraphicsView;

class GUICOREDLL_EXPORT GridAttributeCellSetting : public CompositeContainer
{
public:
	GridAttributeCellSetting();
	GridAttributeCellSetting(const GridAttributeCellSetting& s);

	GridAttributeCellSetting& operator=(const GridAttributeCellSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void apply(vtkActor* actor, VTKGraphicsView* view);

	IntContainer lineWidth;
	OpacityContainer opacity;
};

#endif // GRIDATTRIBUTECELLSETTING_H
