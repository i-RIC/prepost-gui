#include "preprocessorgridshapedataitem_positioneditdialog.h"
#include "ui_preprocessorgridshapedataitem_positioneditdialog.h"

PreProcessorGridShapeDataItem::PositionEditDialog::PositionEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGridShapeDataItem_PositionEditDialog)
{
	ui->setupUi(this);
}

PreProcessorGridShapeDataItem::PositionEditDialog::~PositionEditDialog()
{
	delete ui;
}

QPointF PreProcessorGridShapeDataItem::PositionEditDialog::position() const
{
	return QPointF(ui->x_edit->value(), ui->y_edit->value());
}

void PreProcessorGridShapeDataItem::PositionEditDialog::setPosition(const QPointF& position)
{
	setPosition(position.x(), position.y());
}

void PreProcessorGridShapeDataItem::PositionEditDialog::setPosition(double x, double y)
{
	ui->x_edit->setValue(x);
	ui->y_edit->setValue(y);
}

void PreProcessorGridShapeDataItem::PositionEditDialog::setIndex(int index)
{
	ui->xlabel->setText(tr("x(%1):").arg(index + 1));
	ui->ylabel->setText(tr("y(%1):").arg(index + 1));
}

void PreProcessorGridShapeDataItem::PositionEditDialog::setIJ(int i, int j)
{
	ui->xlabel->setText(tr("x(%1, %2):").arg(i + 1).arg(j + 1));
	ui->ylabel->setText(tr("y(%1, %2):").arg(i + 1).arg(j + 1));
}
