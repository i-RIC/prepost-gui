#ifndef TMSLOADER_TMSREQUESTGSI_IMPL_H
#define TMSLOADER_TMSREQUESTGSI_IMPL_H

#include "../tmsrequestgsi.h"

namespace tmsloader {

class TmsRequestXYZ;

class TmsRequestGSI::Impl
{
public:
	Impl(const QPointF& centerLonLat, const QSize& size, double scale, TmsRequestGSI::TileType tileType);
	~Impl();

	TmsRequestXYZ* m_requestXYZ;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTGSI_IMPL_H
