#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

class iRICAuthClient;

namespace Ui
{
	class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
	Q_OBJECT

public:
	/// @param authClient  When non-null, a "Telemetry" tab is added that lets
	///                    the user revisit the telemetry consent choice.
	explicit PreferenceDialog(QWidget* parent = nullptr, iRICAuthClient* authClient = nullptr);
	~PreferenceDialog();

	bool save();

public slots:
	void accept() override;

private:
	Ui::PreferenceDialog* ui;
};

#endif // PREFERENCEDIALOG_H
