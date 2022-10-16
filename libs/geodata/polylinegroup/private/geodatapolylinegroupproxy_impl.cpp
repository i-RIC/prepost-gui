#include "geodatapolylinegroupproxy_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

GeoDataPolyLineGroupProxy::Impl::Impl()
{
	m_edgesActor = vtkActor::New();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_edgesActor->SetMapper(mapper);
}

GeoDataPolyLineGroupProxy::Impl::~Impl()
{
	m_edgesActor->Delete();
}
