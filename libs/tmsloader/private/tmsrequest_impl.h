#ifndef TMSLOADER_TMSREQUEST_IMPL_H
#define TMSLOADER_TMSREQUEST_IMPL_H

#include "../tmsrequest.h"

#include <QPointF>
#include <QSize>

namespace tmsloader {

class TmsRequest::Impl
{
public:
	Impl(const QPointF& centerLonLat, const QSize& size, double scale);

	QPointF m_center;
	QSize m_size;
	double m_scale;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUEST_IMPL_H
