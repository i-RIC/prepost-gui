#ifndef NETWORKSETTING_H
#define NETWORKSETTING_H

#include <QString>
#include <QNetworkProxyFactory>

class NetworkSetting : public QNetworkProxyFactory
{
public:
	enum class ProxyMode {
		NoProxy,
		SystemSetting,
		CustomSetting
	};

	NetworkSetting();
	~NetworkSetting();

	QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& query = QNetworkProxyQuery()) override;

	void save();

	ProxyMode proxyMode() const;
	void setProxyMode(ProxyMode m);

	QString proxySite() const;
	void setProxySite(const QString& site);

	int proxyPort() const;
	void setProxyPort(int port);

	bool needAuthentication() const;
	void setNeedAuthentication(bool auth);

	QString userName() const;
	void setUserName(const QString& name);

	QString password() const;
	void setPassword(const QString& p);

private:
	void load();

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/networksetting_impl.h"
#endif _DEBUG

#endif // NETWORKSETTING_H
