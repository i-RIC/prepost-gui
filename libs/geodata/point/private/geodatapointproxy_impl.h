#ifndef GEODATAPOINTPROXY_IMPL_H
#define GEODATAPOINTPROXY_IMPL_H

#include "../geodatapointproxy.h"
#include "geodatapointproxy_displaysetting.h"

#include <QImage>

class vtkActor;
class vtkActor2D;

class GeoDataPointProxy::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_pointActor;
	vtkActor2D* m_imageActor;
	QImage m_shrinkedImage;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOINTPROXY_IMPL_H
