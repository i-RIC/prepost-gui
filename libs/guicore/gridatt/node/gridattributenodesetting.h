#ifndef GRIDATTRIBUTENODESETTING_H
#define GRIDATTRIBUTENODESETTING_H

#include "../../guicore_global.h"

#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>

class vtkActor;
class VTKGraphicsView;

class GUICOREDLL_EXPORT GridAttributeNodeSetting : public CompositeContainer
{
public:
	enum class RenderMode {Paint, Point};

	GridAttributeNodeSetting();
	GridAttributeNodeSetting(const GridAttributeNodeSetting& s);

	GridAttributeNodeSetting& operator=(const GridAttributeNodeSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void apply(vtkActor* actor, VTKGraphicsView* view);

	EnumContainerT<RenderMode> renderMode;
	IntContainer lineWidth;
	IntContainer pointSize;
	vtkTextPropertySettingContainer stringSetting;
	OpacityContainer opacity;
};

#endif // GRIDATTRIBUTENODESETTING_H
