#include "preferencedialog.h"
#include "preferencepage.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreferenceDialog)
{
	ui->setupUi(this);
}

PreferenceDialog::~PreferenceDialog()
{
	delete ui;
}

bool PreferenceDialog::save()
{
	int tabs = ui->tabWidget->count();
	bool ngExist = false;
	for (int i = 0; i < tabs; ++i){
		QWidget* tab = ui->tabWidget->widget(i);
		PreferencePage* page = dynamic_cast<PreferencePage*>(tab);
		ngExist = ngExist || (! page->checkSetting());
	}
	if (ngExist){return false;}
	for (int i = 0; i < tabs; ++i){
		QWidget* tab = ui->tabWidget->widget(i);
		PreferencePage* page = dynamic_cast<PreferencePage*>(tab);
		page->update();
	}
	return true;
}

void PreferenceDialog::accept()
{
	bool ok = save();
	if (! ok){return;}
	QDialog::accept();
}
