#include "tmsrequest.h"
#include "private/tmsrequest_impl.h"

using namespace tmsloader;

TmsRequest::Impl::Impl(const QPointF& centerLonLat, const QSize& size, double scale) :
	m_center {centerLonLat},
	m_size {size},
	m_scale {scale}
{}

// public interfaces

TmsRequest::TmsRequest(const QPointF& centerLonLat, const QSize& size, double scale) :
	impl {new Impl {centerLonLat, size, scale}}
{}

TmsRequest::~TmsRequest()
{
	delete impl;
}

QPointF TmsRequest::center() const
{
	return impl->m_center;
}

QSize TmsRequest::size() const
{
	return impl->m_size;
}
double TmsRequest::scale() const
{
	return impl->m_scale;
}
