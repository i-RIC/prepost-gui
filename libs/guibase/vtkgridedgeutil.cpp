#include "vtkgridedgeutil.h"

#include <vtkActor.h>
#include <vtkProperty.h>

void vtkGridEdgeUtil::setupActor(vtkActor* actor)
{
	auto prop = actor->GetProperty();
	prop->SetLineWidth(5);
	prop->SetColor(0, 0, 0);
	prop->SetOpacity(0.6);
}

vtkGridEdgeUtil::vtkGridEdgeUtil()
{}
