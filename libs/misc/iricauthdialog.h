#ifndef IRICAUTHDIALOG_H
#define IRICAUTHDIALOG_H

#include "misc_global.h"

#include <QDialog>

class iRICAuthClient;
class TelemetryWidget;

namespace Ui
{
	class iRICAuthDialog;
}

/// First-run telemetry consent dialog.
///
/// Shown once at startup, when no telemetry choice has been stored yet. It is
/// a thin frame around the shared TelemetryWidget plus OK / Cancel; the same
/// widget is also embedded in the Preferences "Telemetry" tab for later
/// changes. The dialog only borrows the iRICAuthClient; it does not own it.
/// Closing the dialog always lets iRIC continue.
class MISCDLL_EXPORT iRICAuthDialog : public QDialog
{
	Q_OBJECT

public:
	explicit iRICAuthDialog(iRICAuthClient* client, QWidget* parent = nullptr);
	~iRICAuthDialog();

public slots:
	void accept() override;

private:
	TelemetryWidget* m_widget;
	Ui::iRICAuthDialog* ui;
};

#endif // IRICAUTHDIALOG_H
