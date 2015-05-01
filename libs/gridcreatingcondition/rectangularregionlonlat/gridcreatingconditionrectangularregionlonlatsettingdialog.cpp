#include <cmath>

#include "gridcreatingconditionrectangularregionlonlatsettingdialog.h"
#include "ui_gridcreatingconditionrectangularregionlonlatsettingdialog.h"
#include "gridcreatingconditionrectangularregionlonlat.h"

#include <QPushButton>

GridCreatingConditionRectangularRegionLonLatSettingDialog::GridCreatingConditionRectangularRegionLonLatSettingDialog(GridCreatingConditionRectangularRegionLonLat* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRectangularRegionLonLatSettingDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_condition = cond;
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));

	connect(ui->xMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkXMax()));
	connect(ui->xMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkXMin()));
	connect(ui->yMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkYMax()));
	connect(ui->yMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkYMin()));

	connect(ui->xMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateResultDisplays()));
	connect(ui->yMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateResultDisplays()));
	connect(ui->xMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateResultDisplays()));
	connect(ui->yMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateResultDisplays()));
	connect(ui->stepSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateResultDisplays()));
}

GridCreatingConditionRectangularRegionLonLatSettingDialog::~GridCreatingConditionRectangularRegionLonLatSettingDialog()
{
	delete ui;
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::setXMin(double xmin)
{
	ui->xMinSpinBox->setValue(xmin);
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::setXMax(double xmax)
{
	ui->xMaxSpinBox->setValue(xmax);
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::setYMin(double ymin)
{
	ui->yMinSpinBox->setValue(ymin);
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::setYMax(double ymax)
{
	ui->yMaxSpinBox->setValue(ymax);
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::setStepSize(double size)
{
	ui->stepSizeSpinBox->setValue(size);
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::checkXMin()
{
	if (! isVisible()) {return;}
	if (ui->xMinSpinBox->value() > ui->xMaxSpinBox->value()) {
		ui->xMinSpinBox->setValue(ui->xMaxSpinBox->value());
		updateResultDisplays();
	}
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::checkXMax()
{
	if (! isVisible()) {return;}
	if (ui->xMaxSpinBox->value() < ui->xMinSpinBox->value()) {
		ui->xMaxSpinBox->setValue(ui->xMinSpinBox->value());
		updateResultDisplays();
	}
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::checkYMin()
{
	if (! isVisible()) {return;}
	if (ui->yMinSpinBox->value() > ui->yMaxSpinBox->value()) {
		ui->yMinSpinBox->setValue(ui->yMaxSpinBox->value());
		updateResultDisplays();
	}
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::checkYMax()
{
	if (! isVisible()) {return;}
	if (ui->yMaxSpinBox->value() < ui->yMinSpinBox->value()) {
		ui->yMaxSpinBox->setValue(ui->yMinSpinBox->value());
		updateResultDisplays();
	}
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::apply()
{
	m_condition->previewGrid(
		ui->xMinSpinBox->value(), ui->xMaxSpinBox->value(),
		ui->yMinSpinBox->value(), ui->yMaxSpinBox->value(),
		ui->stepSizeSpinBox->value());
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::updateResultDisplays()
{
	// stepSize
	double stepSize = ui->stepSizeSpinBox->value();
	// inum
	unsigned int inum = floor((ui->xMaxSpinBox->value() - ui->xMinSpinBox->value()) / stepSize) + 1;
	ui->resultINumDisplay->setValue(inum);
	// jnum
	unsigned int jnum = floor((ui->yMaxSpinBox->value() - ui->yMinSpinBox->value()) / stepSize) + 1;
	ui->resultJNumDisplay->setValue(jnum);
	// xmin
	double xmin = ui->xMinSpinBox->value();
	ui->resultXMinDisplay->setValue(xmin);
	// xmax
	double xmax = xmin + stepSize * (inum - 1);
	ui->resultXMaxDisplay->setValue(xmax);

	// ymin
	double ymin = ui->yMinSpinBox->value();
	ui->resultYMinDisplay->setValue(ymin);
	// ymax
	double ymax = ymin + stepSize * (jnum - 1);
	ui->resultYMaxDisplay->setValue(ymax);

	bool ng = (xmin >= xmax || ymin >= ymax);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(ng);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(ng);
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::accept()
{
	bool ret = m_condition->createGrid(
		ui->xMinSpinBox->value(), ui->xMaxSpinBox->value(),
		ui->yMinSpinBox->value(), ui->yMaxSpinBox->value(),
		ui->stepSizeSpinBox->value());
	if (! ret) {return;}
	QDialog::accept();
}

void GridCreatingConditionRectangularRegionLonLatSettingDialog::showEvent(QShowEvent* e)
{
	updateResultDisplays();
	QDialog::showEvent(e);
}
