#include "ui_gridcreatingconditioncenterandwidthdialog.h"

#include "gridcreatingconditioncenterandwidthdialog.h"

#include <QMessageBox>

GridCreatingConditionCenterAndWidthDialog::GridCreatingConditionCenterAndWidthDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionCenterAndWidthDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
	connect(ui->nISpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateDI()));
	connect(ui->nJSpinBox, SIGNAL(editingFinished()), this, SLOT(handleNJChange()));
	connect(ui->widthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateDJ()));
}

GridCreatingConditionCenterAndWidthDialog::~GridCreatingConditionCenterAndWidthDialog()
{
	delete ui;
}

int GridCreatingConditionCenterAndWidthDialog::iMax() const
{
	return ui->nISpinBox->value() + 1;
}

int GridCreatingConditionCenterAndWidthDialog::jMax() const
{
	return ui->nJSpinBox->value() + 1;
}

double GridCreatingConditionCenterAndWidthDialog::width() const
{
	return ui->widthSpinBox->value();
}

void GridCreatingConditionCenterAndWidthDialog::setIMax(int i)
{
	ui->nISpinBox->setValue(i - 1);
	updateDI();
}

void GridCreatingConditionCenterAndWidthDialog::setJMax(int j)
{
	ui->nJSpinBox->setValue(j - 1);
	updateDJ();
}

void GridCreatingConditionCenterAndWidthDialog::setWidth(double w)
{
	ui->widthSpinBox->setValue(w);
}

void GridCreatingConditionCenterAndWidthDialog::setLength(double l)
{
	m_length = l;
}

void GridCreatingConditionCenterAndWidthDialog::updateDI()
{
	double d = m_length / (iMax() - 1);
	QString num = QString::number(d);
	ui->dIValue->setText(QString("%1").arg(num));
}

void GridCreatingConditionCenterAndWidthDialog::updateDJ()
{
	double d = width() / (jMax() - 1);
	QString num = QString::number(d);
	ui->dJValue->setText(QString("%1").arg(num));
}

void GridCreatingConditionCenterAndWidthDialog::handleNJChange()
{
	if (ui->nJSpinBox->value() % 2 == 1) {
		QMessageBox::warning(this, tr("Warning"), tr("nJ should be even. incrementing by one."), QMessageBox::Ok, QMessageBox::Ok);
		setJMax(jMax() + 1);
		return;
	}

	updateDJ();
}

void GridCreatingConditionCenterAndWidthDialog::apply()
{
	updateDI();
	updateDJ();
	emit applied(this);
}

void GridCreatingConditionCenterAndWidthDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}
