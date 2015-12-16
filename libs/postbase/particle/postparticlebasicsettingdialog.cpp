#include "ui_postparticlebasicsettingdialog.h"

#include "postparticlebasicsettingdialog.h"

PostParticleBasicSettingDialog::PostParticleBasicSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PostParticleBasicSettingDialog}
{
	ui->setupUi(this);
}

PostParticleBasicSettingDialog::~PostParticleBasicSettingDialog()
{
	delete ui;
}

PostParticleBasicSetting PostParticleBasicSettingDialog::setting() const
{
	PostParticleBasicSetting ret;

	ret.color = ui->colorWidget->color();
	ret.size = ui->sizeSpinBox->value();

	return ret;
}

void PostParticleBasicSettingDialog::setSetting(const PostParticleBasicSetting& s)
{
	ui->colorWidget->setColor(s.color);
	ui->sizeSpinBox->setValue(s.size);
}
