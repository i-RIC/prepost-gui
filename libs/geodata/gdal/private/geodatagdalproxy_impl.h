#ifndef GEODATAGDALPROXY_IMPL_H
#define GEODATAGDALPROXY_IMPL_H

#include "../geodatagdalproxy.h"
#include "geodatagdalproxy_displaysetting.h"

class vtkActor;

class GeoDataGdalProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_actor;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAGDALPROXY_IMPL_H
