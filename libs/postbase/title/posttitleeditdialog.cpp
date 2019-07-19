#include "ui_posttitleeditdialog.h"

#include "posttitleeditdialog.h"

PostTitleEditDialog::PostTitleEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PostTitleEditDialog)
{
	ui->setupUi(this);
}

PostTitleEditDialog::~PostTitleEditDialog()
{
	delete ui;
}

void PostTitleEditDialog::setEnabled(bool /*enabled*/)
{

}

bool PostTitleEditDialog::isEnabled() const
{
	return true;
}

void PostTitleEditDialog::setSetting(const PostTitleSetting& setting)
{
	ui->titleLineEdit->setText(setting.title);
	ui->fontSettingWidget->setSetting(setting.fontSetting);
}

const PostTitleSetting PostTitleEditDialog::setting()
{
	PostTitleSetting ret;
	ret.title = ui->titleLineEdit->text().trimmed();
	ret.fontSetting = ui->fontSettingWidget->setting();
	return ret;
}
