#include "preferencepagetmsadddialog.h"
#include "ui_preferencepagetmsadddialog.h"

#include <guicore/tmsimage/tmsimagesetting.h>
#include <guicore/tmsimage/tmsimagesettingmanager.h>

PreferencePageTmsAddDialog::PreferencePageTmsAddDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreferencePageTmsAddDialog)
{
	ui->setupUi(this);
}

PreferencePageTmsAddDialog::~PreferencePageTmsAddDialog()
{
	delete ui;
}

TmsImageSetting PreferencePageTmsAddDialog::setting()
{
	TmsImageSettingManager manager;
	return manager.setupXYZSetting(ui->nameEdit->text(), ui->urlEdit->text(), ui->maxZoomSpinBox->value());
}
