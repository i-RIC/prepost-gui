#ifndef IRICAUTHCLIENT_IMPL_H
#define IRICAUTHCLIENT_IMPL_H

#include "../iricauthclient.h"

#include <QString>
#include <QUrl>

class QTcpServer;

class iRICAuthClient::Impl
{
public:
	QUrl m_baseUrl;
	QString m_iricVersion;
	QString m_clientId;

	QString m_accessToken;
	QString m_refreshToken;
	QString m_email;
	QString m_userId;

	// interactive-login transient state
	QString m_codeVerifier;
	QString m_state;
	QTcpServer* m_callbackServer {nullptr};
	quint16 m_callbackPort {0};
	bool m_loginFinished {false};
};

#endif // IRICAUTHCLIENT_IMPL_H
