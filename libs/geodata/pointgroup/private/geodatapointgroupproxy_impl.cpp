#include "geodatapointgroupproxy_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

GeoDataPointGroupProxy::Impl::Impl()
{
	m_pointsActor = vtkActor::New();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_pointsActor->SetMapper(mapper);
}

GeoDataPointGroupProxy::Impl::~Impl()
{
	m_pointsActor->Delete();
}
