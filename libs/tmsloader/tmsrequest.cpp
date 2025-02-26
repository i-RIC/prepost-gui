#include "tmsrequest.h"
#include "private/tmsrequest_impl.h"

using namespace tmsloader;

TmsRequest::Impl::Impl(const QPointF& centerLonLat, const QSize& size, int zoomLevel, bool lonLat) :
	m_center {centerLonLat},
	m_size {size},
	m_zoomLevel {zoomLevel},
	m_lonLat {lonLat}
{}

// public interfaces

TmsRequest::TmsRequest(const QPointF& centerLonLat, const QSize& size, int zoomLevel, bool lonLat) :
	impl {new Impl {centerLonLat, size, zoomLevel, lonLat}}
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

int TmsRequest::zoomLevel() const
{
	return impl->m_zoomLevel;
}

bool TmsRequest::lonLat() const
{
	return impl->m_lonLat;
}
