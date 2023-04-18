#include "geodatapointproxy_impl.h"

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

GeoDataPointProxy::Impl::Impl() :
	m_pointActor {vtkActor::New()},
	m_imageActor {vtkActor2D::New()}
{
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_pointActor->SetMapper(mapper);
}

GeoDataPointProxy::Impl::~Impl()
{
	m_pointActor->Delete();
	m_imageActor->Delete();
}
