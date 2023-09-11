#include "vtkgraphicsviewscalewidget_scaleeditdialog.h"
#include "ui_vtkgraphicsviewscalewidget_scaleeditdialog.h"

VtkGraphicsViewScaleWidget::ScaleEditDialog::ScaleEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VtkGraphicsViewScaleWidget_ScaleEditDialog)
{
	ui->setupUi(this);
}

VtkGraphicsViewScaleWidget::ScaleEditDialog::~ScaleEditDialog()
{
	delete ui;
}

double VtkGraphicsViewScaleWidget::ScaleEditDialog::scale() const
{
	return ui->scaleSpinBox->value();
}

void VtkGraphicsViewScaleWidget::ScaleEditDialog::setScale(double scale)
{
	ui->scaleSpinBox->setValue(scale);
}
