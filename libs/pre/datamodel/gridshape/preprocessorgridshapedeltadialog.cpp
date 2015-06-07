#include "ui_preprocessorgridshapedeltadialog.h"

#include "preprocessorgridshapedeltadialog.h"

PreProcessorGridShapeDeltaDialog::PreProcessorGridShapeDeltaDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGridShapeDeltaDialog)
{
	ui->setupUi(this);
	ui->x_edit->setValue(0);
	ui->y_edit->setValue(0);
}

PreProcessorGridShapeDeltaDialog::~PreProcessorGridShapeDeltaDialog()
{
	delete ui;
}

QVector2D PreProcessorGridShapeDeltaDialog::delta()
{
	return QVector2D(ui->x_edit->value(), ui->y_edit->value());
}
