#include "scalarbardialog.h"
#include "ui_scalarbardialog.h"

ScalarBarDialog::ScalarBarDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::ScalarBarDialog)
{
	ui->setupUi(this);
}

ScalarBarDialog::~ScalarBarDialog()
{
	delete ui;
}

void ScalarBarDialog::setSetting(const ScalarBarSetting& setting)
{
	ui->widget->setSetting(setting);
}

void ScalarBarDialog::setTitle(const QString& title)
{
	ui->widget->setTitle(title);
}

void ScalarBarDialog::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	ui->widget->setTitleTextSetting(cont);
}

void ScalarBarDialog::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	ui->widget->setLabelTextSetting(cont);
}

const ScalarBarSetting ScalarBarDialog::setting()
{
	return ui->widget->setting();
}

QString ScalarBarDialog::title()
{
	return ui->widget->title();
}

const vtkTextPropertySettingContainer& ScalarBarDialog::titleTextSetting() const
{
	return ui->widget->titleTextSetting();
}

const vtkTextPropertySettingContainer& ScalarBarDialog::labelTextSetting() const
{
	return ui->widget->labelTextSetting();
}
