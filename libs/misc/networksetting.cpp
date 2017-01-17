#include "networksetting.h"
#include "private/networksetting_impl.h"

#include <QSettings>

NetworkSetting::NetworkSetting() :
	QNetworkProxyFactory(),
	impl {new Impl {}}
{
	load();
}

NetworkSetting::~NetworkSetting()
{
	delete impl;
}

QList<QNetworkProxy> NetworkSetting::queryProxy(const QNetworkProxyQuery&)
{
	QList<QNetworkProxy> ret;
	switch (impl->m_proxyMode) {
	case ProxyMode::NoProxy: {
		QNetworkProxy proxy;
		proxy.setType(QNetworkProxy::NoProxy);
		ret.append(proxy);
		}
		break;

	case ProxyMode::SystemSetting:
		ret.append(QNetworkProxyFactory::systemProxyForQuery());
		break;

	case ProxyMode::CustomSetting: {
		QNetworkProxy proxy;
		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(impl->m_proxySite);
		proxy.setPort(impl->m_proxyPort);

		if (impl->m_needAuthentication) {
			proxy.setUser(impl->m_userName);
			proxy.setPassword(impl->m_password);
		}
		ret.append(proxy);
		}
		break;
	}

	return ret;
}

void NetworkSetting::save()
{
	QSettings settings("iRIC Organization", "iRIC GUI");
	settings.setValue("networkproxy/mode", static_cast<int>(impl->m_proxyMode));
	settings.setValue("networkproxy/site", impl->m_proxySite);
	settings.setValue("networkproxy/port", impl->m_proxyPort);

	settings.setValue("networkproxy/auth", impl->m_needAuthentication);
	settings.setValue("networkproxy/username", impl->m_userName);
	settings.setValue("networkproxy/password", impl->m_password);
}

NetworkSetting::ProxyMode NetworkSetting::proxyMode() const
{
	return impl->m_proxyMode;
}

void NetworkSetting::setProxyMode(ProxyMode m)
{
	impl->m_proxyMode = m;
}

QString NetworkSetting::proxySite() const
{
	return impl->m_proxySite;
}

void NetworkSetting::setProxySite(const QString& site)
{
	impl->m_proxySite = site;
}

int NetworkSetting::proxyPort() const
{
	return impl->m_proxyPort;
}

void NetworkSetting::setProxyPort(int port)
{
	impl->m_proxyPort = port;
}

bool NetworkSetting::needAuthentication() const
{
	return impl->m_needAuthentication;
}

void NetworkSetting::setNeedAuthentication(bool auth)
{
	impl->m_needAuthentication = auth;
}

QString NetworkSetting::userName() const
{
	return impl->m_userName;
}

void NetworkSetting::setUserName(const QString& name)
{
	impl->m_userName = name;
}

QString NetworkSetting::password() const
{
	return impl->m_password;
}

void NetworkSetting::setPassword(const QString& p)
{
	impl->m_password = p;
}

void NetworkSetting::load()
{
	QSettings settings("iRIC Organization", "iRIC GUI");
	impl->m_proxyMode = static_cast<ProxyMode>(settings.value("networkproxy/mode", 0).toInt());
	impl->m_proxySite = settings.value("networkproxy/site").toString();
	impl->m_proxyPort = settings.value("networkproxy/port", 80).toInt();

	impl->m_needAuthentication = settings.value("networkproxy/auth", false).toBool();
	impl->m_userName = settings.value("networkproxy/username").toString();
	impl->m_password = settings.value("networkproxy/password").toString();
}
