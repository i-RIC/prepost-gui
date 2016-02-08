#ifndef TMSLOADER_TMSREQUESTBING_IMPL_H
#define TMSLOADER_TMSREQUESTBING_IMPL_H

#include "../tmsrequestbing.h"

namespace tmsloader {

class TmsRequestBing::Impl
{
public:
	Impl(TmsRequestBing::ImagerySet imagerySet);

	TmsRequestBing::ImagerySet m_imagerySet;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTBING_IMPL_H

