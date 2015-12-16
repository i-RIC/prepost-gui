#include "ui_scalarbardialog.h"

#include "scalarbardialog.h"

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

ScalarBarSetting ScalarBarDialog::setting() const
{
	return ui->widget->setting();
}

void ScalarBarDialog::setSetting(const ScalarBarSetting& setting)
{
	ui->widget->setSetting(setting);
}

QString ScalarBarDialog::title() const
{
	return ui->widget->title();
}

void ScalarBarDialog::setTitle(const QString& title)
{
	ui->widget->setTitle(title);
}
