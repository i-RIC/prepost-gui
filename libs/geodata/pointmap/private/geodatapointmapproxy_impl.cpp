#include "geodatapointmapproxy_impl.h"

#include <vtkLODActor.h>
#include <vtkProperty.h>

GeoDataPointmapProxy::Impl::Impl() :
	m_tinActor {vtkLODActor::New()}
{
	auto prop = m_tinActor->GetProperty();
	prop->SetRepresentationToPoints();
	prop->SetLighting(false);
}

GeoDataPointmapProxy::Impl::~Impl()
{
	m_tinActor->Delete();
}
