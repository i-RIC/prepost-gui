#include "geodatanetcdfproxy_impl.h"

GeoDataNetcdfProxy::Impl::Impl() :
	m_actor {vtkActor::New()}
{}

GeoDataNetcdfProxy::Impl::~Impl()
{
	m_actor->Delete();
}
