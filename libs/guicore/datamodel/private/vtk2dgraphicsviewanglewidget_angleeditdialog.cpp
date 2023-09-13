#include "vtk2dgraphicsviewanglewidget_angleeditdialog.h"
#include "ui_vtk2dgraphicsviewanglewidget_angleeditdialog.h"

Vtk2dGraphicsViewAngleWidget::AngleEditDialog::AngleEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Vtk2dGraphicsViewAngleWidget_AngleEditDialog)
{
	ui->setupUi(this);
}

Vtk2dGraphicsViewAngleWidget::AngleEditDialog::~AngleEditDialog()
{
	delete ui;
}

double Vtk2dGraphicsViewAngleWidget::AngleEditDialog::angle() const
{
	return ui->angleSpinBox->value();
}

void Vtk2dGraphicsViewAngleWidget::AngleEditDialog::setAngle(double angle)
{
	while (angle > 360) {
		angle -= 360;
	}
	while (angle < 0) {
		angle += 360;
	}

	ui->angleSpinBox->setValue(angle);
}
