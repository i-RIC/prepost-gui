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
	ui->fontSizeSpinBox->setValue(setting.fontSize);
	ui->colorWidget->setColor(setting.color);
}

const PostTitleSetting PostTitleEditDialog::setting()
{
	PostTitleSetting ret;
	ret.title = ui->titleLineEdit->text().trimmed();
	ret.fontSize = ui->fontSizeSpinBox->value();
	ret.color = ui->colorWidget->color();
	return ret;
}
