#ifndef GEODATANETCDFPROXY_IMPL_H
#define GEODATANETCDFPROXY_IMPL_H

#include "../geodatanetcdfproxy.h"
#include "geodatanetcdfproxy_displaysetting.h"

class vtkActor;

class GeoDataNetcdfProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_actor;

	DisplaySetting m_displaySetting;
};

#endif // GEODATANETCDFPROXY_IMPL_H
