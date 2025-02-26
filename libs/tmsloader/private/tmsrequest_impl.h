#ifndef TMSLOADER_TMSREQUEST_IMPL_H
#define TMSLOADER_TMSREQUEST_IMPL_H

#include "../tmsrequest.h"

#include <QPointF>
#include <QSize>

namespace tmsloader {

class TmsRequest::Impl
{
public:
	Impl(const QPointF& centerLonLat, const QSize& size, int zoomLevel, bool lonLat);

	QPointF m_center;
	QSize m_size;
	int m_zoomLevel;
	bool m_lonLat;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUEST_IMPL_H
