#ifndef GEODATAPOLYGONGROUPPROXY_IMPL_H
#define GEODATAPOLYGONGROUPPROXY_IMPL_H

#include "../geodatapolygongroupproxy.h"
#include "geodatapolygongroupproxy_displaysetting.h"

class ColorMapSettingContainerI;

class vtkAppendPolyData;
class vtkPolyData;

class GeoDataPolygonGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	static void setupActors(vtkActor* edgesActor, vtkPolyData* edgesData, vtkActor* paintActor, vtkPolyData* paintData, const GeoDataPolygonGroup::DisplaySetting& ds, ColorMapSettingContainerI* cm);

	vtkActor* m_edgesActor;
	vtkActor* m_paintActor;

	vtkActor* m_editTargetEdgesActor;
	vtkActor* m_editTargetPaintActor;
	vtkAppendPolyData* m_editTargetEdgesPolyData;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYGONGROUPPROXY_IMPL_H
