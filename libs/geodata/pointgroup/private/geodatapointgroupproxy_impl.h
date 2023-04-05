#ifndef GEODATAPOINTGROUPPROXY_IMPL_H
#define GEODATAPOINTGROUPPROXY_IMPL_H

#include "../geodatapointgroupproxy.h"
#include "geodatapointgroupproxy_displaysetting.h"

#include <vector>

#include <QImage>

class vtkActor2D;

class GeoDataPointGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_pointsActor;
	std::vector<vtkActor2D*> m_imageActors;
	QImage m_shrinkedImage;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOINTGROUPPROXY_IMPL_H
