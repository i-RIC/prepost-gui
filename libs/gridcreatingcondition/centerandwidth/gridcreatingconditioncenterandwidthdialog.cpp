#include "ui_gridcreatingconditioncenterandwidthdialog.h"

#include "gridcreatingconditioncenterandwidthdialog.h"

#include <QMessageBox>

GridCreatingConditionCenterAndWidthDialog::GridCreatingConditionCenterAndWidthDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionCenterAndWidthDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
	connect(ui->streamWiseSpinBox, SIGNAL(editingFinished()), this, SLOT(setStreamWiseNumber()));
	connect(ui->crossStreamSpinBox, SIGNAL(editingFinished()), this, SLOT(setCrossStreamNumber()));
	connect(ui->gridWidthSpinBox, SIGNAL(editingFinished()), this, SLOT(setStreamWiseLabel()));
	connect(ui->gridWidthSpinBox, SIGNAL(editingFinished()), this, SLOT(setCrossStreamLabel()));
}

GridCreatingConditionCenterAndWidthDialog::~GridCreatingConditionCenterAndWidthDialog()
{
	delete ui;
}

int GridCreatingConditionCenterAndWidthDialog::iMax()
{
	return ui->streamWiseSpinBox->value() + 1;
}

int GridCreatingConditionCenterAndWidthDialog::jMax()
{
	return ui->crossStreamSpinBox->value() + 1;
}

double GridCreatingConditionCenterAndWidthDialog::width()
{
	return ui->gridWidthSpinBox->value();
}

double GridCreatingConditionCenterAndWidthDialog::length()
{
	return m_length;
}

void GridCreatingConditionCenterAndWidthDialog::setIMax(int i)
{
	ui->streamWiseSpinBox->setValue(i - 1);
}

void GridCreatingConditionCenterAndWidthDialog::setJMax(int j)
{
	ui->crossStreamSpinBox->setValue(j - 1);
}

void GridCreatingConditionCenterAndWidthDialog::setWidth(double w)
{
	ui->gridWidthSpinBox->setValue(w);
}

void GridCreatingConditionCenterAndWidthDialog::setLength(double l)
{
	m_length = l;
}

void GridCreatingConditionCenterAndWidthDialog::setStreamWiseLabel()
{
	double d = length() / (iMax() - 1);
	QString num = QString::number(d);
	ui->dIValue->setText(QString("%1").arg(num));
}

void GridCreatingConditionCenterAndWidthDialog::setCrossStreamLabel()
{
	double d = width() / (jMax() - 1);
	QString num = QString::number(d);
	ui->dJValue->setText(QString("%1").arg(num));
}

void GridCreatingConditionCenterAndWidthDialog::setStreamWiseNumber()
{
	setStreamWiseLabel();
}

void GridCreatingConditionCenterAndWidthDialog::setCrossStreamNumber()
{
	if (ui->crossStreamSpinBox->value() % 2 == 1) {
		QMessageBox::warning(this, tr("Warning"),
            tr("Division number in cross-stream direction should be even. incrementing by one."),
			QMessageBox::Ok, QMessageBox::Ok);
		setJMax(jMax() + 1);
	}
	setCrossStreamLabel();
}

void GridCreatingConditionCenterAndWidthDialog::apply()
{
	setStreamWiseLabel();
	setCrossStreamLabel();
	emit applied(this);
}

void GridCreatingConditionCenterAndWidthDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}
