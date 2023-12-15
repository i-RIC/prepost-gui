#include "ui_gridcreatingconditioncenterandwidthdialog.h"

#include "gridcreatingconditioncenterandwidthdialog.h"

#include <QMessageBox>
#include <QPushButton>

GridCreatingConditionCenterAndWidthDialog::GridCreatingConditionCenterAndWidthDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionCenterAndWidthDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
	connect<void (QSpinBox::*)(int)> (ui->nISpinBox, &QSpinBox::valueChanged, this, &GridCreatingConditionCenterAndWidthDialog::updateDI);
	connect<void (QSpinBox::*)(int)> (ui->nJSpinBox, &QSpinBox::valueChanged, this, &GridCreatingConditionCenterAndWidthDialog::updateDJ);
	connect(ui->nJSpinBox, &QSpinBox::editingFinished, this, &GridCreatingConditionCenterAndWidthDialog::handleNJChange);
	connect<void (QDoubleSpinBox::*)(double)>(ui->widthSpinBox, &QDoubleSpinBox::valueChanged, this, &GridCreatingConditionCenterAndWidthDialog::updateDJ);

	ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Create Grid"));
}

GridCreatingConditionCenterAndWidthDialog::~GridCreatingConditionCenterAndWidthDialog()
{
	delete ui;
}

void GridCreatingConditionCenterAndWidthDialog::setReadOnly(bool readOnly)
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(readOnly);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(readOnly);
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
