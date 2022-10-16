#ifndef GEODATAPOLYGONGROUPPROXY_IMPL_H
#define GEODATAPOLYGONGROUPPROXY_IMPL_H

#include "../geodatapolygongroupproxy.h"

class GeoDataPolygonGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_edgesActor;
	vtkActor* m_paintActor;

	GeoDataPolygonGroupColorSettingDialog::Setting m_setting;
};

#endif // GEODATAPOLYGONGROUPPROXY_IMPL_H
