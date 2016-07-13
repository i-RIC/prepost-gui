#ifndef TMSLOADER_TMSREQUESTXYZ_IMPL_H
#define TMSLOADER_TMSREQUESTXYZ_IMPL_H

#include "../tmsrequestxyz.h"

#include <QString>

namespace tmsloader {

class TmsRequestXYZ::Impl
{
public:
	Impl(const QString& url, const std::map<QString, QString>& options);

	QString m_url;
	std::map<QString, QString> m_options;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTXYZ_IMPL_H
