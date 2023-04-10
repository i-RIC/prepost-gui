#ifndef GEODATAPOLYGONGROUPPROXY_IMPL_H
#define GEODATAPOLYGONGROUPPROXY_IMPL_H

#include "../geodatapolygongroupproxy.h"
#include "geodatapolygongroupproxy_displaysetting.h"

class GeoDataPolygonGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_edgesActor;
	vtkActor* m_paintActor;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYGONGROUPPROXY_IMPL_H
