#include "gridcreatingconditionlaplacectrlpointsdialog.h"
#include "ui_gridcreatingconditionlaplacectrlpointsdialog.h"

#include <QFile>
#include <QPixmap>

GridCreatingConditionLaplaceCtrlPointsDialog::GridCreatingConditionLaplaceCtrlPointsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionLaplaceCtrlPointsDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionLaplaceCtrlPointsDialog::~GridCreatingConditionLaplaceCtrlPointsDialog()
{
	delete ui;
}

void GridCreatingConditionLaplaceCtrlPointsDialog::setLocale(const QLocale& locale)
{
	QString fname = QString(":/libs/gridcreatingcondition/laplace/images/%1/imgCtrlCrossSections.png").arg(locale.name());
	QFile file(fname);
	if (! file.exists()) {return;}

	ui->imgLabel->setPixmap(QPixmap(fname));
}

void GridCreatingConditionLaplaceCtrlPointsDialog::setMaximum(int max)
{
	ui->numSpinBox->setMaximum(max);
	int num = max / 3;
	ui->numSpinBox->setValue(num);
}

int GridCreatingConditionLaplaceCtrlPointsDialog::value() const
{
	return ui->numSpinBox->value();
}

void GridCreatingConditionLaplaceCtrlPointsDialog::reject()
{}
