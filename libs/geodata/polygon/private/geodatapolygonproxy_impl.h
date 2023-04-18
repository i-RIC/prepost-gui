#ifndef GEODATAPOLYGONPROXY_IMPL_H
#define GEODATAPOLYGONPROXY_IMPL_H

#include "../geodatapolygonproxy.h"
#include "geodatapolygonproxy_displaysetting.h"

class vtkAppendPolyData;

class GeoDataPolygonProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_edgesActor;
	vtkActor* m_paintActor;
	vtkAppendPolyData* m_edgesPolyData;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYGONPROXY_IMPL_H
