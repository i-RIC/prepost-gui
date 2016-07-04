#ifndef NETWORKSETTING_IMPL_H
#define NETWORKSETTING_IMPL_H

#include "../networksetting.h"

class NetworkSetting::Impl
{
public:
	ProxyMode m_proxyMode;
	QString m_proxySite;
	int m_proxyPort;

	bool m_needAuthentication;
	QString m_userName;
	QString m_password;
};

#endif // NETWORKSETTING_IMPL_H

