#include "ui_preprocessorgridshapenewpositiondialog.h"

#include "preprocessorgridshapenewpositiondialog.h"

PreProcessorGridShapeNewPositionDialog::PreProcessorGridShapeNewPositionDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGridShapeNewPositionDialog)
{
	ui->setupUi(this);
}

PreProcessorGridShapeNewPositionDialog::~PreProcessorGridShapeNewPositionDialog()
{
	delete ui;
}

const QVector2D PreProcessorGridShapeNewPositionDialog::position()
{
	return QVector2D(ui->x_edit->value(), ui->y_edit->value());
}

void PreProcessorGridShapeNewPositionDialog::setPosition(double x, double y)
{
	ui->x_edit->setValue(x);
	ui->y_edit->setValue(y);
}

void PreProcessorGridShapeNewPositionDialog::setIndex(int index)
{
	ui->xlabel->setText(tr("x(%1):").arg(index + 1));
	ui->ylabel->setText(tr("y(%1):").arg(index + 1));
}

void PreProcessorGridShapeNewPositionDialog::setIJ(int i, int j)
{
	ui->xlabel->setText(tr("x(%1, %2):").arg(i + 1).arg(j + 1));
	ui->ylabel->setText(tr("y(%1, %2):").arg(i + 1).arg(j + 1));
}
