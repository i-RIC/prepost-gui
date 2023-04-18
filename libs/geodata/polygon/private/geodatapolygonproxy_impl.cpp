#include "geodatapolygonproxy_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkAppendPolyData.h>
#include <vtkSmartPointer.h>

GeoDataPolygonProxy::Impl::Impl() :
	m_edgesActor {vtkActor::New()},
	m_paintActor {vtkActor::New()},
	m_edgesPolyData {vtkAppendPolyData::New()}
{
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_edgesActor->SetMapper(mapper);

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_paintActor->SetMapper(mapper);
}

GeoDataPolygonProxy::Impl::~Impl()
{
	m_edgesActor->Delete();
	m_paintActor->Delete();
	m_edgesPolyData->Delete();
}
