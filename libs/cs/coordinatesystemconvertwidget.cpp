#include "coordinatesystem.h"
#include "coordinatesystemconvertwidget.h"
#include "coordinatesystemselectdialog.h"
#include "ui_coordinatesystemconvertwidget.h"

CoordinateSystemConvertWidget::CoordinateSystemConvertWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CoordinateSystemConvertWidget)
{
	ui->setupUi(this);
}

CoordinateSystemConvertWidget::~CoordinateSystemConvertWidget()
{
	delete ui;
}

void CoordinateSystemConvertWidget::setBuilder(CoordinateSystemBuilder* builder)
{
	m_builder = builder;
}


void CoordinateSystemConvertWidget::setEnabled(bool enabled)
{
	ui->convertCheckBox->show();
	ui->csLabel->show();
	ui->editButton->show();
	ui->disabledLabel->show();

	if (enabled) {
		ui->disabledLabel->hide();
	} else {
		ui->convertCheckBox->hide();
		ui->csLabel->hide();
		ui->editButton->hide();
	}
}

CoordinateSystem* CoordinateSystemConvertWidget::coordinateSystem() const
{
	return nullptr;
}

void CoordinateSystemConvertWidget::setCoordinateSystem(CoordinateSystem* cs)
{
	m_coordinateSystem = cs;
	ui->csLabel->setText(cs->caption());
}

void CoordinateSystemConvertWidget::edit()
{
	CoordinateSystemSelectDialog dialog(this);
	dialog.setForceSelect(true);
	dialog.setCoordinateSystem(m_coordinateSystem);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	setCoordinateSystem(dialog.coordinateSystem());
}
