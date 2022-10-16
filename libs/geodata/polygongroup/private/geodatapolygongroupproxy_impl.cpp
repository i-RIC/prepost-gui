#include "geodatapolygongroupproxy_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

GeoDataPolygonGroupProxy::Impl::Impl()
{
	m_edgesActor = vtkActor::New();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_edgesActor->SetMapper(mapper);

	m_paintActor = vtkActor::New();

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_paintActor->SetMapper(mapper);
}

GeoDataPolygonGroupProxy::Impl::~Impl()
{
	m_edgesActor->Delete();
	m_paintActor->Delete();
}
