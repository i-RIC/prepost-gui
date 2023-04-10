#ifndef GEODATAPOINTMAPPROXY_IMPL_H
#define GEODATAPOINTMAPPROXY_IMPL_H

#include "../geodatapointmapproxy.h"
#include "geodatapointmapproxy_displaysetting.h"

class vtkLODActor;

class GeoDataPointmapProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkLODActor* m_tinActor;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOINTMAPPROXY_IMPL_H
