#ifndef GEODATAPOINTGROUPPROXY_IMPL_H
#define GEODATAPOINTGROUPPROXY_IMPL_H

#include "../geodatapointgroupproxy.h"

class GeoDataPointGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_pointsActor;

	GeoDataPointGroupColorSettingDialog::Setting m_setting;
};

#endif // GEODATAPOINTGROUPPROXY_IMPL_H
