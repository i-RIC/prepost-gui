#ifndef TMSLOADER_TMSREQUESTGOOGLEMAP_IMPL_H
#define TMSLOADER_TMSREQUESTGOOGLEMAP_IMPL_H

#include "../tmsrequestgooglemap.h"

namespace tmsloader {

class TmsRequestGoogleMap::Impl
{
public:
	Impl(TmsRequestGoogleMap::MapType mapType);

	TmsRequestGoogleMap::MapType m_mapType;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTGOOGLEMAP_IMPL_H
