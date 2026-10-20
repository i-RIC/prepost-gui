#include "geodatapolylinegroupproxy_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

GeoDataPolyLineGroupProxy::Impl::Impl()
{
	m_edgesActor = vtkActor::New();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_edgesActor->SetMapper(mapper);

	m_editTargetActor = vtkActor::New();

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_editTargetActor->SetMapper(mapper);
}

GeoDataPolyLineGroupProxy::Impl::~Impl()
{
	m_edgesActor->Delete();
	m_editTargetActor->Delete();
}
