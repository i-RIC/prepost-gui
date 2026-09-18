#include "ui_preferencedialog.h"

#include "preferencedialog.h"
#include "preferencepage.h"
#include "preferencepagetelemetry.h"

PreferenceDialog::PreferenceDialog(QWidget* parent, iRICAuthClient* authClient) :
	QDialog(parent),
	ui(new Ui::PreferenceDialog)
{
	ui->setupUi(this);

	// The Telemetry tab is added only when an auth client is available. The
	// no-arg construction used by iRICMainWindow::initSetting() (to persist
	// default values) must not gain this tab, otherwise it would write
	// "telemetry/mode" before the user has been asked.
	if (authClient != nullptr) {
		ui->tabWidget->addTab(new PreferencePageTelemetry(authClient, this), tr("Telemetry"));
	}
}

PreferenceDialog::~PreferenceDialog()
{
	delete ui;
}

bool PreferenceDialog::save()
{
	int tabs = ui->tabWidget->count();
	bool ngExist = false;
	for (int i = 0; i < tabs; ++i) {
		QWidget* tab = ui->tabWidget->widget(i);
		PreferencePage* page = dynamic_cast<PreferencePage*>(tab);
		ngExist = ngExist || (! page->checkSetting());
	}
	if (ngExist) {return false;}
	for (int i = 0; i < tabs; ++i) {
		QWidget* tab = ui->tabWidget->widget(i);
		PreferencePage* page = dynamic_cast<PreferencePage*>(tab);
		page->update();
	}
	return true;
}

void PreferenceDialog::accept()
{
	bool ok = save();
	if (! ok) {return;}
	QDialog::accept();
}
