#include "../../datamodel/vtkgraphicsview.h"
#include "gridattributenodesetting.h"

#include <vtkActor.h>
#include <vtkProperty.h>

GridAttributeNodeSetting::GridAttributeNodeSetting() :
	CompositeContainer({&renderMode, &lineWidth, &pointSize, &opacity}),
	renderMode {"renderMode", RenderMode::Paint},
	lineWidth {"lineWidth", 5},
	pointSize {"pointSize", 5},
	stringSetting {},
	opacity {}
{
	opacity = 50;
}

GridAttributeNodeSetting::GridAttributeNodeSetting(const GridAttributeNodeSetting& s) :
	GridAttributeNodeSetting {}
{
	copyValue(s);
}

GridAttributeNodeSetting& GridAttributeNodeSetting::operator=(const GridAttributeNodeSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GridAttributeNodeSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const GridAttributeNodeSetting&> (c));
}

void GridAttributeNodeSetting::apply(vtkActor* actor, VTKGraphicsView* view)
{
	auto prop = actor->GetProperty();

	if (renderMode == RenderMode::Paint) {
		prop->SetRepresentationToSurface();
	} else if (renderMode == RenderMode::Point) {
		prop->SetRepresentationToPoints();
	}

	prop->SetLineWidth(lineWidth * view->devicePixelRatioF());
	prop->SetPointSize(pointSize * view->devicePixelRatioF());
	prop->SetOpacity(opacity);
}
