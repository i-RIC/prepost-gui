#ifndef TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H
#define TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H

#include "tmsrequesthandler.h"

namespace tmsloader {

class TmsRequestHandlerOpenStreetMap : public TmsRequestHandler
{
public:
	TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWidget* parent);
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H
