#ifndef GEODATAPOLYLINEGROUPPROXY_IMPL_H
#define GEODATAPOLYLINEGROUPPROXY_IMPL_H

#include "../geodatapolylinegroupproxy.h"
#include "geodatapolylinegroupproxy_displaysetting.h"

class ColorMapSettingContainerI;

class vtkPolyData;

class GeoDataPolyLineGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	static void setupEdgesActor(vtkActor* actor, vtkPolyData* data, const GeoDataPolyLineGroup::DisplaySetting& ds, ColorMapSettingContainerI* cm);

	vtkActor* m_edgesActor;
	vtkActor* m_editTargetActor;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYLINEGROUPPROXY_IMPL_H
