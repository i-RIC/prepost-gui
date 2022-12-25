#ifndef GEODATAPOLYLINEGROUPPROXY_IMPL_H
#define GEODATAPOLYLINEGROUPPROXY_IMPL_H

#include "../geodatapolylinegroupproxy.h"

class GeoDataPolyLineGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_edgesActor;

	GeoDataPolyLineGroupColorSettingDialog::Setting m_setting;
};

#endif // GEODATAPOLYLINEGROUPPROXY_IMPL_H
