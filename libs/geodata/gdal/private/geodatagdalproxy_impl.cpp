#include "geodatagdalproxy_impl.h"

GeoDataGdalProxy::Impl::Impl() :
	m_actor {vtkActor::New()}
{}

GeoDataGdalProxy::Impl::~Impl()
{
	m_actor->Delete();
}
