#ifndef GEODATAPOLYLINEGROUPPROXY_IMPL_H
#define GEODATAPOLYLINEGROUPPROXY_IMPL_H

#include "../geodatapolylinegroupproxy.h"
#include "geodatapolylinegroupproxy_displaysetting.h"

class GeoDataPolyLineGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_edgesActor;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYLINEGROUPPROXY_IMPL_H
