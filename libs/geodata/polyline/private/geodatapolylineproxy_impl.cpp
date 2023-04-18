#include "geodatapolylineproxy_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

GeoDataPolyLineProxy::Impl::Impl() :
	m_edgesActor {vtkActor::New()}
{
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_edgesActor->SetMapper(mapper);
}

GeoDataPolyLineProxy::Impl::~Impl()
{
	m_edgesActor->Delete();
}
