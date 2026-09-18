#include "iricauthclient.h"
#include "private/iricauthclient_impl.h"

#include "authcredentialstore.h"
#include "machineinfo.h"
#include "qttool.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QDebug>
#include <QDesktopServices>
#include <QHostAddress>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPair>
#include <QRandomGenerator>
#include <QSettings>
#include <QStringList>
#include <QSysInfo>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QUrlQuery>
#include <QUuid>

namespace {

const char* CLIENT_ID = "iric-desktop";
const char* CALLBACK_PATH = "/callback";
const char* PATH_AUTHORIZE = "/authorize";
const char* PATH_TOKEN = "/api/oauth/token";
const char* PATH_TELEMETRY = "/api/telemetry/events";
const char* PATH_TELEMETRY_MACHINE = "/api/telemetry/machine";

const char* SETTINGS_TELEMETRY_MODE = "telemetry/mode";
const char* SETTINGS_TELEMETRY_ANON_ID = "telemetry/anonId";

// Values written under SETTINGS_TELEMETRY_MODE.
const char* MODE_NONE = "none";
const char* MODE_ANONYMOUS = "anonymous";
const char* MODE_LOGIN = "login";

// Telemetry strings are capped by the hub; keep the wire small and match that.
const int TELEMETRY_VALUE_MAX = 200;

// The interactive browser login is abandoned after this long.
const int INTERACTIVE_TIMEOUT_MS = 5 * 60 * 1000;
// A single token request is aborted after this long.
const int TOKEN_REQUEST_TIMEOUT_MS = 30 * 1000;

QByteArray randomBytes(int len)
{
	QByteArray b;
	b.resize(len);
	for (int i = 0; i < len; ++i) {
		b[i] = static_cast<char>(QRandomGenerator::system()->bounded(256));
	}
	return b;
}

QString base64Url(const QByteArray& b)
{
	return QString::fromLatin1(b.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

// Decode (without verifying) a JWT payload. Signature verification is the
// responsibility of the hub API that receives the token.
QJsonObject jwtPayload(const QString& jwt)
{
	const QStringList parts = jwt.split('.');
	if (parts.size() != 3) {return QJsonObject();}
	const QByteArray json = QByteArray::fromBase64(parts.at(1).toUtf8(), QByteArray::Base64UrlEncoding);
	return QJsonDocument::fromJson(json).object();
}

QNetworkAccessManager* nam()
{
	return QtTool::networkAccessManager();
}

// Serialize parameters as an application/x-www-form-urlencoded body.
QByteArray formEncode(const QList<QPair<QString, QString> >& params)
{
	QByteArray out;
	for (const QPair<QString, QString>& p : params) {
		if (! out.isEmpty()) {out.append('&');}
		out.append(QUrl::toPercentEncoding(p.first));
		out.append('=');
		out.append(QUrl::toPercentEncoding(p.second));
	}
	return out;
}

// Device attribution attached to every token / refresh request (see auth_sample).
void appendDeviceParams(QList<QPair<QString, QString> >& params, const QString& iricVersion)
{
	params << qMakePair(QString("device_label"), QSysInfo::machineHostName());
	params << qMakePair(QString("os"), QString("windows"));
	params << qMakePair(QString("iric_version"), iricVersion);
}

iRICAuthClient::TelemetryMode modeFromString(const QString& s)
{
	if (s == QLatin1String(MODE_NONE)) {return iRICAuthClient::TelemetryMode::None;}
	if (s == QLatin1String(MODE_ANONYMOUS)) {return iRICAuthClient::TelemetryMode::Anonymous;}
	if (s == QLatin1String(MODE_LOGIN)) {return iRICAuthClient::TelemetryMode::Login;}
	return iRICAuthClient::TelemetryMode::Unset;
}

QString modeToString(iRICAuthClient::TelemetryMode mode)
{
	switch (mode) {
	case iRICAuthClient::TelemetryMode::None:      return QString::fromLatin1(MODE_NONE);
	case iRICAuthClient::TelemetryMode::Anonymous: return QString::fromLatin1(MODE_ANONYMOUS);
	case iRICAuthClient::TelemetryMode::Login:     return QString::fromLatin1(MODE_LOGIN);
	case iRICAuthClient::TelemetryMode::Unset:     return QString();
	}
	return QString();
}

// Truncate to the hub's per-value limit (it would trim the overflow anyway).
QString clip(const QString& s)
{
	return s.left(TELEMETRY_VALUE_MAX);
}

} // namespace

QUrl iRICAuthClient::defaultBaseUrl()
{
	return QUrl(QStringLiteral("https://id.i-ric.org"));
}

iRICAuthClient::TelemetryMode iRICAuthClient::telemetryMode()
{
	QSettings settings;
	return modeFromString(settings.value(QString::fromLatin1(SETTINGS_TELEMETRY_MODE)).toString());
}

void iRICAuthClient::setTelemetryMode(TelemetryMode mode)
{
	QSettings settings;
	if (mode == TelemetryMode::Unset) {
		settings.remove(QString::fromLatin1(SETTINGS_TELEMETRY_MODE));
	} else {
		settings.setValue(QString::fromLatin1(SETTINGS_TELEMETRY_MODE), modeToString(mode));
	}
}

QString iRICAuthClient::anonId()
{
	QSettings settings;
	const QString key = QString::fromLatin1(SETTINGS_TELEMETRY_ANON_ID);
	QString id = settings.value(key).toString();
	if (id.isEmpty()) {
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
		settings.setValue(key, id);
	}
	return id;
}

iRICAuthClient::iRICAuthClient(const QUrl& baseUrl, const QString& iricVersion, QObject* parent) :
	QObject(parent),
	impl {new Impl {}}
{
	impl->m_baseUrl = baseUrl.isValid() && ! baseUrl.isEmpty() ? baseUrl : defaultBaseUrl();
	impl->m_iricVersion = iricVersion;
	impl->m_clientId = QString::fromLatin1(CLIENT_ID);
}

iRICAuthClient::~iRICAuthClient()
{
	closeCallbackServer();
	delete impl;
}

bool iRICAuthClient::hasStoredCredential() const
{
	return ! iRICAuth::loadRefreshToken().isEmpty();
}

bool iRICAuthClient::isLoggedIn() const
{
	return ! impl->m_accessToken.isEmpty();
}

QString iRICAuthClient::accessToken() const
{
	return impl->m_accessToken;
}

QString iRICAuthClient::email() const
{
	return impl->m_email;
}

QString iRICAuthClient::userId() const
{
	return impl->m_userId;
}

QString iRICAuthClient::redirectUri() const
{
	return QString("http://127.0.0.1:%1%2").arg(impl->m_callbackPort).arg(QString::fromLatin1(CALLBACK_PATH));
}

void iRICAuthClient::trySilentLogin()
{
	const QString refreshToken = iRICAuth::loadRefreshToken();
	if (refreshToken.isEmpty()) {
		finishLogin(false, tr("Not signed in."));
		return;
	}

	QList<QPair<QString, QString> > params;
	params << qMakePair(QString("grant_type"), QString("refresh_token"));
	params << qMakePair(QString("refresh_token"), refreshToken);
	params << qMakePair(QString("client_id"), impl->m_clientId);
	appendDeviceParams(params, impl->m_iricVersion);

	sendTokenRequest(formEncode(params), false);
}

void iRICAuthClient::startInteractiveLogin()
{
	if (impl->m_callbackServer != nullptr) {
		// A login is already in progress.
		return;
	}
	impl->m_loginFinished = false;

	impl->m_codeVerifier = base64Url(randomBytes(32));
	impl->m_state = base64Url(randomBytes(16));
	const QString challenge = base64Url(QCryptographicHash::hash(impl->m_codeVerifier.toLatin1(), QCryptographicHash::Sha256));

	auto* server = new QTcpServer(this);
	if (! server->listen(QHostAddress::LocalHost, 0)) {
		delete server;
		finishLogin(false, tr("Could not start the local sign-in listener."));
		return;
	}
	impl->m_callbackServer = server;
	impl->m_callbackPort = server->serverPort();

	connect(server, &QTcpServer::newConnection, this, [this, server]() {
		QTcpSocket* socket = server->nextPendingConnection();
		if (socket == nullptr) {return;}
		connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
			const QByteArray request = socket->readAll();
			if (! request.contains("\r\n\r\n") && ! request.contains("\n\n")) {
				return; // wait for the rest of the request headers
			}
			const QByteArray body =
				"<!doctype html><html><head><meta charset=\"utf-8\"></head><body>"
				"<p>iRIC \343\201\270\343\201\256\343\202\265\343\202\244\343\203\263\343\202\244\343\203\263\343\201\214\345\256\214\344\272\206\343\201\227\343\201\276\343\201\227\343\201\237\343\200\202"
				"\343\201\223\343\201\256\343\202\277\343\203\226\343\202\222\351\226\211\343\201\230\343\201\246 iRIC \343\201\253\346\210\273\343\201\243\343\201\246\343\201\217\343\201\240\343\201\225\343\201\204\343\200\202</p>"
				"</body></html>";
			QByteArray response = "HTTP/1.1 200 OK\r\n";
			response += "Content-Type: text/html; charset=utf-8\r\n";
			response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
			response += "Connection: close\r\n\r\n";
			response += body;
			socket->write(response);
			socket->flush();
			socket->disconnectFromHost();

			handleCallbackRequest(request);
		});
		connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
	});

	QTimer::singleShot(INTERACTIVE_TIMEOUT_MS, this, [this]() {
		if (impl->m_callbackServer != nullptr && ! impl->m_loginFinished) {
			closeCallbackServer();
			finishLogin(false, tr("The sign-in was not completed in time."));
		}
	});

	QUrl authUrl = impl->m_baseUrl.resolved(QUrl(QString::fromLatin1(PATH_AUTHORIZE)));
	QUrlQuery q;
	q.addQueryItem("response_type", "code");
	q.addQueryItem("client_id", impl->m_clientId);
	q.addQueryItem("redirect_uri", redirectUri());
	q.addQueryItem("code_challenge", challenge);
	q.addQueryItem("code_challenge_method", "S256");
	q.addQueryItem("state", impl->m_state);
	authUrl.setQuery(q);

	QDesktopServices::openUrl(authUrl);
}

void iRICAuthClient::handleCallbackRequest(const QByteArray& request)
{
	if (impl->m_loginFinished) {return;}

	// First line: "GET /callback?code=...&state=... HTTP/1.1"
	const int sp1 = request.indexOf(' ');
	const int sp2 = request.indexOf(' ', sp1 + 1);
	QString target;
	if (sp1 >= 0 && sp2 > sp1) {
		target = QString::fromLatin1(request.mid(sp1 + 1, sp2 - sp1 - 1));
	}
	const QUrl url(QStringLiteral("http://127.0.0.1") + target);
	const QUrlQuery q(url);

	const QString error = q.queryItemValue("error", QUrl::FullyDecoded);
	const QString code = q.queryItemValue("code", QUrl::FullyDecoded);
	const QString state = q.queryItemValue("state", QUrl::FullyDecoded);

	closeCallbackServer();

	if (! error.isEmpty()) {
		finishLogin(false, tr("Sign-in was denied (%1).").arg(error));
		return;
	}
	if (state != impl->m_state) {
		finishLogin(false, tr("Sign-in response validation failed."));
		return;
	}
	if (code.isEmpty()) {
		finishLogin(false, tr("Sign-in response did not contain an authorization code."));
		return;
	}

	emit authorizationCodeReceived();

	QList<QPair<QString, QString> > params;
	params << qMakePair(QString("grant_type"), QString("authorization_code"));
	params << qMakePair(QString("code"), code);
	params << qMakePair(QString("redirect_uri"), redirectUri());
	params << qMakePair(QString("client_id"), impl->m_clientId);
	params << qMakePair(QString("code_verifier"), impl->m_codeVerifier);
	appendDeviceParams(params, impl->m_iricVersion);

	sendTokenRequest(formEncode(params), true);
}

void iRICAuthClient::cancelInteractiveLogin()
{
	if (impl->m_callbackServer == nullptr || impl->m_loginFinished) {
		// Nothing to cancel, or the token exchange is already under way.
		return;
	}
	closeCallbackServer();
	finishLogin(false, tr("Sign-in was cancelled."));
}

void iRICAuthClient::sendTokenRequest(const QByteArray& formBody, bool interactive)
{
	QNetworkAccessManager* manager = nam();
	if (manager == nullptr) {
		finishLogin(false, tr("Network is not available."));
		return;
	}

	QNetworkRequest req(impl->m_baseUrl.resolved(QUrl(QString::fromLatin1(PATH_TOKEN))));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	QNetworkReply* reply = manager->post(req, formBody);

	auto* timeout = new QTimer(reply);
	timeout->setSingleShot(true);
	connect(timeout, &QTimer::timeout, reply, &QNetworkReply::abort);
	timeout->start(TOKEN_REQUEST_TIMEOUT_MS);

	connect(reply, &QNetworkReply::finished, this, [this, reply, interactive]() {
		handleTokenReply(reply, interactive);
	});
}

void iRICAuthClient::handleTokenReply(QNetworkReply* reply, bool interactive)
{
	reply->deleteLater();

	if (reply->error() != QNetworkReply::NoError) {
		finishLogin(false, tr("Could not reach the iRIC ID service (%1).").arg(reply->errorString()));
		return;
	}
	applyTokenResponse(reply->readAll(), interactive);
}

void iRICAuthClient::applyTokenResponse(const QByteArray& json, bool interactive)
{
	Q_UNUSED(interactive);

	const QJsonObject obj = QJsonDocument::fromJson(json).object();
	const QString accessToken = obj.value("access_token").toString();
	const QString refreshToken = obj.value("refresh_token").toString();

	if (accessToken.isEmpty()) {
		finishLogin(false, tr("The iRIC ID service did not return an access token."));
		return;
	}

	impl->m_accessToken = accessToken;

	const QJsonObject claims = jwtPayload(accessToken);
	impl->m_email = claims.value("email").toString();
	impl->m_userId = claims.value("sub").toString();

	// Follow refresh-token rotation: persist every new value we are given.
	if (! refreshToken.isEmpty() && refreshToken != iRICAuth::loadRefreshToken()) {
		impl->m_refreshToken = refreshToken;
		iRICAuth::saveRefreshToken(refreshToken);
	}

	finishLogin(true, QString());
}

void iRICAuthClient::finishLogin(bool ok, const QString& reason)
{
	impl->m_loginFinished = true;
	if (ok) {
		emit loginSucceeded();
	} else {
		emit loginFailed(reason);
	}
	emit stateChanged();
}

void iRICAuthClient::closeCallbackServer()
{
	if (impl->m_callbackServer != nullptr) {
		impl->m_callbackServer->close();
		impl->m_callbackServer->deleteLater();
		impl->m_callbackServer = nullptr;
	}
}

void iRICAuthClient::logout()
{
	iRICAuth::removeRefreshToken();
	impl->m_accessToken.clear();
	impl->m_refreshToken.clear();
	impl->m_email.clear();
	impl->m_userId.clear();
	emit loggedOut();
	emit stateChanged();
}

void iRICAuthClient::sendAppLaunchTelemetry()
{
	QJsonObject event;
	event.insert("type", "app_launch");
	event.insert("anon_id", anonId());
	event.insert("iric_version", impl->m_iricVersion);
	event.insert("os", "windows");

	QJsonArray events;
	events.append(event);
	postTelemetry(events);
}

void iRICAuthClient::sendSolverRunTelemetry(const QString& solverId, const QString& solverVersion)
{
	QJsonObject event;
	event.insert("type", "solver_run");
	event.insert("anon_id", anonId());
	event.insert("solver_id", solverId);
	event.insert("solver_version", solverVersion);
	event.insert("iric_version", impl->m_iricVersion);
	event.insert("os", "windows");

	QJsonArray events;
	events.append(event);
	postTelemetry(events);
}

void iRICAuthClient::sendMachineTelemetry()
{
	const TelemetryMode mode = telemetryMode();
	if (mode == TelemetryMode::None || mode == TelemetryMode::Unset) {return;}

	QNetworkAccessManager* manager = nam();
	if (manager == nullptr) {return;}

	const MachineInfo mi = collectMachineInfo();

	QJsonObject body;
	body.insert("anon_id", anonId());
	body.insert("os_name", clip(mi.osName));
	body.insert("os_version", clip(mi.osVersion));
	body.insert("cpu_name", clip(mi.cpuName));
	body.insert("gpu_name", clip(mi.gpuName));
	body.insert("memory_size", clip(mi.memorySize));
	body.insert("iric_version", clip(impl->m_iricVersion));

	QNetworkRequest req(impl->m_baseUrl.resolved(QUrl(QString::fromLatin1(PATH_TELEMETRY_MACHINE))));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	if (isLoggedIn()) {
		req.setRawHeader("Authorization", "Bearer " + impl->m_accessToken.toUtf8());
	}

	QNetworkReply* reply = manager->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
	connect(reply, &QNetworkReply::finished, this, [reply]() {
		const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (code != 204) {
			// Best effort: a telemetry failure never affects the application.
			qWarning() << "iRIC ID machine telemetry failed:" << code << reply->errorString();
		}
		reply->deleteLater();
	});
}

void iRICAuthClient::postTelemetry(const QJsonArray& events)
{
	const TelemetryMode mode = telemetryMode();
	if (mode == TelemetryMode::None || mode == TelemetryMode::Unset) {return;}

	QNetworkAccessManager* manager = nam();
	if (manager == nullptr) {return;}

	QJsonObject root;
	root.insert("events", events);

	QNetworkRequest req(impl->m_baseUrl.resolved(QUrl(QString::fromLatin1(PATH_TELEMETRY))));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	// Bearer is optional: attach it only when signed in, otherwise the events
	// are recorded against the anonymous id alone.
	if (isLoggedIn()) {
		req.setRawHeader("Authorization", "Bearer " + impl->m_accessToken.toUtf8());
	}

	QNetworkReply* reply = manager->post(req, QJsonDocument(root).toJson(QJsonDocument::Compact));
	connect(reply, &QNetworkReply::finished, this, [reply]() {
		const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if (code != 204) {
			// Best effort: a telemetry failure never affects the application.
			qWarning() << "iRIC ID telemetry failed:" << code << reply->errorString();
		}
		reply->deleteLater();
	});
}
