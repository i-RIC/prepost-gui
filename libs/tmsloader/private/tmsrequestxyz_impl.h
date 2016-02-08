#ifndef TMSLOADER_TMSREQUESTXYZ_IMPL_H
#define TMSLOADER_TMSREQUESTXYZ_IMPL_H

#include "../tmsrequestxyz.h"

#include <QString>

namespace tmsloader {

class TmsRequestXYZ::Impl
{
public:
	Impl(const QString& url);

	QString m_url;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTXYZ_IMPL_H
