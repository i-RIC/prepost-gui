#include "../../datamodel/vtkgraphicsview.h"
#include "gridattributecellsetting.h"

#include <vtkActor.h>
#include <vtkProperty.h>

GridAttributeCellSetting::GridAttributeCellSetting() :
	CompositeContainer({&lineWidth, &opacity}),
	lineWidth {"lineWidth", 5},
	opacity {}
{
	opacity = 50;
}

GridAttributeCellSetting::GridAttributeCellSetting(const GridAttributeCellSetting& s) :
	GridAttributeCellSetting {}
{
	copyValue(s);
}

GridAttributeCellSetting& GridAttributeCellSetting::operator=(const GridAttributeCellSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GridAttributeCellSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const GridAttributeCellSetting&> (c));
}

void GridAttributeCellSetting::apply(vtkActor* actor, VTKGraphicsView* view)
{
	auto prop = actor->GetProperty();

	prop->SetLineWidth(lineWidth * view->devicePixelRatioF());
	prop->SetOpacity(opacity);
}
