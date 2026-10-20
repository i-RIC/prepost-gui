#include "geodatapolygongroupproxy_impl.h"

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
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

	m_editTargetEdgesActor = vtkActor::New();

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_editTargetEdgesActor->SetMapper(mapper);

	m_editTargetPaintActor = vtkActor::New();

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_editTargetPaintActor->SetMapper(mapper);

	m_editTargetEdgesPolyData = vtkAppendPolyData::New();
}

GeoDataPolygonGroupProxy::Impl::~Impl()
{
	m_edgesActor->Delete();
	m_paintActor->Delete();
	m_editTargetEdgesActor->Delete();
	m_editTargetPaintActor->Delete();
	m_editTargetEdgesPolyData->Delete();
}
