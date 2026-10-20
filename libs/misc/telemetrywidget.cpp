#include "ui_telemetrywidget.h"

#include "iricauthclient.h"
#include "telemetrywidget.h"

TelemetryWidget::TelemetryWidget(iRICAuthClient* client, QWidget* parent) :
	QWidget(parent),
	m_client {client},
	ui {new Ui::TelemetryWidget}
{
	ui->setupUi(this);

	connect(ui->noneRadio, &QRadioButton::toggled, this, &TelemetryWidget::updateControls);
	connect(ui->anonRadio, &QRadioButton::toggled, this, &TelemetryWidget::updateControls);
	connect(ui->loginRadio, &QRadioButton::toggled, this, &TelemetryWidget::updateControls);
	connect(ui->signInButton, &QPushButton::clicked, this, &TelemetryWidget::onSignInButtonClicked);

	if (m_client != nullptr) {
		connect(m_client, &iRICAuthClient::authorizationCodeReceived, this, &TelemetryWidget::handleAuthorizationCodeReceived);
		connect(m_client, &iRICAuthClient::loginSucceeded, this, &TelemetryWidget::handleLoginSucceeded);
		connect(m_client, &iRICAuthClient::loginFailed, this, &TelemetryWidget::handleLoginFailed);
		connect(m_client, &iRICAuthClient::loggedOut, this, &TelemetryWidget::handleLoggedOut);
	}

	loadFromSettings();
}

TelemetryWidget::~TelemetryWidget()
{
	delete ui;
}

void TelemetryWidget::loadFromSettings()
{
	switch (iRICAuthClient::telemetryMode()) {
	case iRICAuthClient::TelemetryMode::None:
		ui->noneRadio->setChecked(true);
		break;
	case iRICAuthClient::TelemetryMode::Login:
		ui->loginRadio->setChecked(true);
		break;
	case iRICAuthClient::TelemetryMode::Anonymous:
	case iRICAuthClient::TelemetryMode::Unset:
		ui->anonRadio->setChecked(true);
		break;
	}
	updateControls();
}

iRICAuthClient::TelemetryMode TelemetryWidget::selectedMode() const
{
	if (ui->noneRadio->isChecked()) {return iRICAuthClient::TelemetryMode::None;}
	if (ui->loginRadio->isChecked()) {return iRICAuthClient::TelemetryMode::Login;}
	return iRICAuthClient::TelemetryMode::Anonymous;
}

bool TelemetryWidget::applyToSettings()
{
	const iRICAuthClient::TelemetryMode mode = selectedMode();
	iRICAuthClient::setTelemetryMode(mode);

	// Leaving "signed in" behind means the stored credential is no longer wanted.
	if (mode != iRICAuthClient::TelemetryMode::Login &&
		m_client != nullptr && m_client->isLoggedIn()) {
		m_client->logout();
	}
	return true;
}

void TelemetryWidget::updateControls()
{
	const bool loginChosen = ui->loginRadio->isChecked();
	const bool loggedIn = (m_client != nullptr) && m_client->isLoggedIn();

	ui->signInButton->setVisible(loginChosen);
	ui->signInButton->setEnabled(loginChosen && (m_client != nullptr));
	ui->signInButton->setText(loggedIn ? tr("Sign out") : tr("Sign in with iRIC ID..."));

	if (loginChosen && loggedIn) {
		ui->statusLabel->setText(tr("Signed in as %1.").arg(m_client->email().isEmpty() ? m_client->userId() : m_client->email()));
	} else if (! loginChosen) {
		ui->statusLabel->clear();
	}
}

void TelemetryWidget::onSignInButtonClicked()
{
	if (m_client == nullptr) {return;}

	if (m_client->isLoggedIn()) {
		m_client->logout();
		return;
	}

	ui->signInButton->setEnabled(false);
	ui->statusLabel->setText(tr("A sign-in page has opened in your web browser. "
								"Complete the sign-in there, then return to iRIC."));
	m_client->startInteractiveLogin();
}

void TelemetryWidget::handleAuthorizationCodeReceived()
{
	ui->statusLabel->setText(tr("Received your sign-in. Contacting the iRIC ID service..."));
}

void TelemetryWidget::handleLoginSucceeded()
{
	updateControls();
}

void TelemetryWidget::handleLoginFailed(const QString& reason)
{
	ui->signInButton->setEnabled(ui->loginRadio->isChecked() && (m_client != nullptr));
	ui->statusLabel->setText(reason.isEmpty() ? tr("Sign-in failed.") : reason);
}

void TelemetryWidget::handleLoggedOut()
{
	updateControls();
	ui->statusLabel->setText(tr("You are signed out."));
}
