#ifndef GEODATAPOINTGROUPPROXY_IMPL_H
#define GEODATAPOINTGROUPPROXY_IMPL_H

#include "../geodatapointgroupproxy.h"
#include "geodatapointgroupproxy_displaysetting.h"

#include <vector>

#include <QImage>

class ColorMapSettingContainerI;

class vtkActor2D;
class vtkPolyData;

class GeoDataPointGroupProxy::Impl
{
public:
	Impl();
	~Impl();

	static void setupPointsActor(vtkActor* actor, vtkPolyData* data, const GeoDataPointGroup::DisplaySetting& ds, ColorMapSettingContainerI* cm);

	vtkActor* m_pointsActor;
	vtkActor* m_editTargetPointActor;
	std::vector<vtkActor2D*> m_imageActors;
	QImage m_shrinkedImage;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOINTGROUPPROXY_IMPL_H
