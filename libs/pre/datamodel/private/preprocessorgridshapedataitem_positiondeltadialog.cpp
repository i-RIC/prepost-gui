#include "preprocessorgridshapedataitem_positiondeltadialog.h"
#include "ui_preprocessorgridshapedataitem_positiondeltadialog.h"

PreProcessorGridShapeDataItem::PositionDeltaDialog::PositionDeltaDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGridShapeDataItem_PositionDeltaDialog)
{
	ui->setupUi(this);
	ui->xEditWidget->setValue(0);
	ui->yEditWidget->setValue(0);
}

PreProcessorGridShapeDataItem::PositionDeltaDialog::~PositionDeltaDialog()
{
	delete ui;
}

QPointF PreProcessorGridShapeDataItem::PositionDeltaDialog::delta() const
{
	return QPointF(ui->xEditWidget->value(), ui->yEditWidget->value());
}
