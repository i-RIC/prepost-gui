#include "offsetsettingdialog.h"
#include "ui_offsetsettingdialog.h"
#include <QMessageBox>

OffsetSettingDialog::OffsetSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::OffsetSettingDialog)
{
	ui->setupUi(this);
}

OffsetSettingDialog::~OffsetSettingDialog()
{
	delete ui;
}

void OffsetSettingDialog::setOffset(double x, double y)
{
	ui->xEdit->setValue(x);
	ui->yEdit->setValue(y);
}

double OffsetSettingDialog::offsetX() const
{
	return ui->xEdit->value();
}

double OffsetSettingDialog::offsetY() const
{
	return ui->yEdit->value();
}

void OffsetSettingDialog::accept()
{
	bool xOK = isPrecisionGood(ui->xEdit->value());
	if (! xOK){
		QMessageBox::warning(this, tr("Warning"), tr("X value is not appropriate. Please input more simple value."));
		return;
	}

	bool yOK = isPrecisionGood(ui->yEdit->value());
	if (! yOK){
		QMessageBox::warning(this, tr("Warning"), tr("Y value is not appropriate. Please input more simple value."));
		return;
	}

	QDialog::accept();
}

bool OffsetSettingDialog::isPrecisionGood(double v){
	QString vstr = QString::number(v);
	double v2 = vstr.toDouble();
	return (v == v2);
}
