#include "coordinatesystemconvertdialog.h"
#include "ui_coordinatesystemconvertdialog.h"

CoordinateSystemConvertDialog::CoordinateSystemConvertDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CoordinateSystemConvertDialog)
{
	ui->setupUi(this);
}

CoordinateSystemConvertDialog::~CoordinateSystemConvertDialog()
{
	delete ui;
}

void CoordinateSystemConvertDialog::setBuilder(CoordinateSystemBuilder* builder)
{
	ui->widget->setBuilder(builder);
}

void CoordinateSystemConvertDialog::setEnabled(bool enabled)
{
	ui->widget->setEnabled(enabled);
}

CoordinateSystem* CoordinateSystemConvertDialog::coordinateSystem() const
{
	return ui->widget->coordinateSystem();
}

void CoordinateSystemConvertDialog::setCoordinateSystem(CoordinateSystem* cs)
{
	ui->widget->setCoordinateSystem(cs);
}
