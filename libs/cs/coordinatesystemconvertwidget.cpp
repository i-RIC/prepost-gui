#include "coordinatesystem.h"
#include "coordinatesystemconvertwidget.h"
#include "coordinatesystemselectdialog.h"
#include "ui_coordinatesystemconvertwidget.h"

CoordinateSystemConvertWidget::CoordinateSystemConvertWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CoordinateSystemConvertWidget)
{
	ui->setupUi(this);
	connect(ui->editButton, &QPushButton::clicked, this, &CoordinateSystemConvertWidget::edit);

	setEnabled(false);
	setCoordinateSystem(nullptr);
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
	ui->editButton->setEnabled(enabled);
}

CoordinateSystem* CoordinateSystemConvertWidget::coordinateSystem() const
{
	return m_coordinateSystem;
}

void CoordinateSystemConvertWidget::setCoordinateSystem(CoordinateSystem* cs)
{
	m_coordinateSystem = cs;
	if (cs == nullptr) {
		ui->csLabel->setText("-----");
	} else {
		ui->csLabel->setText(cs->caption());
	}
}

void CoordinateSystemConvertWidget::edit()
{
	CoordinateSystemSelectDialog dialog(this);
	dialog.setBuilder(m_builder);
	dialog.setForceSelect(true);
	dialog.setCoordinateSystem(m_coordinateSystem);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	setCoordinateSystem(dialog.coordinateSystem());
}
