#include "networksetting.h"
#include <QSettings>

NetworkSetting::NetworkSetting()
	: QNetworkProxyFactory()
{
	load();
}

void NetworkSetting::load()
{
	QSettings settings("iRIC Organization", "iRIC GUI");
	m_proxyMode = static_cast<ProxyMode>(settings.value("networkproxy/mode", 0).toInt());
	m_proxySite = settings.value("networkproxy/site").toString();
	m_proxyPort = settings.value("networkproxy/port", 80).toInt();

	m_needAuthentication = settings.value("networkproxy/auth", false).toBool();
	m_userName = settings.value("networkproxy/username").toString();
	m_password = settings.value("networkproxy/password").toString();
}

void NetworkSetting::save()
{
	QSettings settings("iRIC Organization", "iRIC GUI");
	settings.setValue("networkproxy/mode", static_cast<int>(m_proxyMode));
	settings.setValue("networkproxy/site", m_proxySite);
	settings.setValue("networkproxy/port", m_proxyPort);

	settings.setValue("networkproxy/auth", m_needAuthentication);
	settings.setValue("networkproxy/username", m_userName);
	settings.setValue("networkproxy/password", m_password);
}

QList<QNetworkProxy> NetworkSetting::queryProxy(const QNetworkProxyQuery& query)
{
	QList<QNetworkProxy> ret;
	switch (m_proxyMode) {
	case pmNoProxy: {
			QNetworkProxy proxy;
			proxy.setType(QNetworkProxy::NoProxy);
			ret.append(proxy);
		}
		break;

	case pmSystemSetting:
		ret.append(QNetworkProxyFactory::systemProxyForQuery());
		break;

	case pmCustomSetting: {
			QNetworkProxy proxy;
			proxy.setType(QNetworkProxy::HttpProxy);
			proxy.setHostName(m_proxySite);
			proxy.setPort(m_proxyPort);

			if (m_needAuthentication) {
				proxy.setUser(m_userName);
				proxy.setPassword(m_password);
			}
			ret.append(proxy);
		}
		break;
	}

	return ret;
}
