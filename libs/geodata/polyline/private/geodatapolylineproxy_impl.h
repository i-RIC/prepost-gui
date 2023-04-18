#ifndef GEODATAPOLYLINEPROXY_IMPL_H
#define GEODATAPOLYLINEPROXY_IMPL_H

#include "../geodatapolylineproxy.h"
#include "geodatapolylineproxy_displaysetting.h"

class vtkAppendPolyData;

class GeoDataPolyLineProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_edgesActor;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYLINEPROXY_IMPL_H
