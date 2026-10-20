#ifndef IRICAUTHCLIENT_H
#define IRICAUTHCLIENT_H

#include "misc_global.h"

#include <QObject>
#include <QString>
#include <QUrl>

class QByteArray;
class QJsonArray;
class QNetworkReply;

/// iRIC ID authentication client.
///
/// Implements the OAuth2 Authorization Code flow with PKCE (S256) for the
/// public client "iric-desktop", plus best-effort telemetry. The interactive
/// login opens the system browser and receives the redirect on a loopback
/// HTTP listener (127.0.0.1, OS-assigned port). The refresh token is stored
/// through iRICAuth::saveRefreshToken() so that subsequent launches can log
/// in silently.
///
/// This is a hand-rolled equivalent of the QtNetworkAuth-based reference
/// implementation in auth_sample/, because iRIC GUI builds against Qt 5.14
/// which does not ship QtNetworkAuth.
class MISCDLL_EXPORT iRICAuthClient : public QObject
{
	Q_OBJECT

public:
	/// How much the user agreed to share for telemetry. Persisted as
	/// "telemetry/mode" in QSettings.
	enum class TelemetryMode {
		Unset,      ///< The user has not chosen yet (show the consent dialog).
		None,       ///< Do not send anything.
		Anonymous,  ///< Send with anon_id only, no iRIC ID sign-in.
		Login,      ///< Send and attach the signed-in user's bearer token.
	};

	/// The production hub URL used when nothing overrides it.
	static QUrl defaultBaseUrl();

	/// The stored telemetry consent. Returns TelemetryMode::Unset when nothing
	/// has been chosen yet.
	static TelemetryMode telemetryMode();
	static void setTelemetryMode(TelemetryMode mode);

	/// A per-installation anonymous UUID (RFC 4122, lower-case, no braces),
	/// created and persisted on first use. Shared by every telemetry payload.
	static QString anonId();

	/// @param baseUrl      Hub base URL (e.g. https://id.i-ric.org).
	/// @param iricVersion  iRIC GUI version string, reported to the hub.
	iRICAuthClient(const QUrl& baseUrl, const QString& iricVersion, QObject* parent = nullptr);
	~iRICAuthClient();

	/// True when a refresh token is stored from a previous session.
	bool hasStoredCredential() const;

	/// True while a valid access token is held in memory.
	bool isLoggedIn() const;

	QString accessToken() const;
	QString email() const;
	QString userId() const;

public slots:
	/// Try to log in silently using the stored refresh token. Emits
	/// loginSucceeded() or loginFailed() when finished. Returns immediately
	/// (does nothing) when no refresh token is stored.
	void trySilentLogin();

	/// Start the interactive browser login.
	void startInteractiveLogin();

	/// Abort an interactive login that is still waiting for the browser
	/// redirect. No-op once the token exchange has started.
	void cancelInteractiveLogin();

	/// Forget the stored refresh token and the in-memory tokens.
	void logout();

	/// Best-effort "app_launch" telemetry. No-op unless telemetry is enabled.
	void sendAppLaunchTelemetry();

	/// Best-effort "solver_run" telemetry. No-op unless telemetry is enabled.
	void sendSolverRunTelemetry(const QString& solverId, const QString& solverVersion);

	/// Best-effort machine-configuration telemetry (POST /api/telemetry/machine).
	/// Sends the OS / CPU / GPU / memory description once. No-op unless telemetry
	/// is enabled; sent anonymously when not signed in.
	void sendMachineTelemetry();

signals:
	void loginSucceeded();
	void loginFailed(const QString& reason);
	void loggedOut();
	/// Emitted whenever the logged-in state may have changed.
	void stateChanged();
	/// Emitted after the browser redirect is received, just before the
	/// authorization code is exchanged for tokens (progress feedback).
	void authorizationCodeReceived();

private:
	QString redirectUri() const;
	void sendTokenRequest(const QByteArray& formBody, bool interactive);
	void handleTokenReply(QNetworkReply* reply, bool interactive);
	void applyTokenResponse(const QByteArray& json, bool interactive);
	void handleCallbackRequest(const QByteArray& request);
	void closeCallbackServer();
	void postTelemetry(const QJsonArray& events);
	void finishLogin(bool ok, const QString& reason);

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/iricauthclient_impl.h"
#endif // _DEBUG

#endif // IRICAUTHCLIENT_H
