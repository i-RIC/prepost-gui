#ifndef NETWORKSETTING_H
#define NETWORKSETTING_H

#include <QString>
#include <QNetworkProxyFactory>

class NetworkSetting : public QNetworkProxyFactory
{
public:
	enum ProxyMode {
		pmNoProxy,
		pmSystemSetting,
		pmCustomSetting
	};

	NetworkSetting();
	QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& query = QNetworkProxyQuery());

	void save();

	ProxyMode proxyMode() const {return m_proxyMode;}
	QString proxySite() const {return m_proxySite;}
	int proxyPort() const {return m_proxyPort;}

	bool needAuthentication() const {return m_needAuthentication;}
	QString userName() const {return m_userName;}
	QString password() const {return m_password;}

	void setProxyMode(ProxyMode m) {m_proxyMode = m;}
	void setProxySite(const QString& site) {m_proxySite = site;}
	void setProxyPort(int port) {m_proxyPort = port;}

	void setNeedAuthentication(bool auth) {m_needAuthentication = auth;}
	void setUserName(const QString& name) {m_userName = name;}
	void setPassword(const QString& p) {m_password = p;}

private:
	void load();

	ProxyMode m_proxyMode;
	QString m_proxySite;
	int m_proxyPort;

	bool m_needAuthentication;
	QString m_userName;
	QString m_password;
};

#endif // NETWORKSETTING_H
