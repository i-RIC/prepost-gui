#ifndef TELEMETRYWIDGET_H
#define TELEMETRYWIDGET_H

#include "misc_global.h"

#include "iricauthclient.h"

#include <QWidget>

namespace Ui
{
	class TelemetryWidget;
}

/// The telemetry consent chooser, shared by the first-run dialog and the
/// Preferences "Telemetry" tab.
///
/// Presents the three-way choice - do not participate / participate
/// anonymously / participate signed in with iRIC ID - together with the list
/// of exactly what is and is not sent. The iRIC ID sign-in is offered inline
/// (its own button) so the widget behaves identically in both hosts. The
/// widget borrows the iRICAuthClient; it does not own it, and it tolerates a
/// null client (the sign-in button is simply disabled).
class MISCDLL_EXPORT TelemetryWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TelemetryWidget(iRICAuthClient* client, QWidget* parent = nullptr);
	~TelemetryWidget();

	/// Preselect the radio button matching the stored telemetry mode. When
	/// nothing is stored yet, "participate anonymously" is preselected.
	void loadFromSettings();

	/// Persist the selected mode. Switching away from "signed in" signs the
	/// user out. Always returns true (sign-in is handled inline, so the host
	/// dialog never needs to stay open).
	bool applyToSettings();

private slots:
	void updateControls();
	void onSignInButtonClicked();
	void handleAuthorizationCodeReceived();
	void handleLoginSucceeded();
	void handleLoginFailed(const QString& reason);
	void handleLoggedOut();

private:
	iRICAuthClient::TelemetryMode selectedMode() const;

	iRICAuthClient* m_client;
	Ui::TelemetryWidget* ui;
};

#endif // TELEMETRYWIDGET_H
