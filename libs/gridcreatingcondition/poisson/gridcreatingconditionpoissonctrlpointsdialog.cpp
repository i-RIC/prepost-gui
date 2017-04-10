#include "gridcreatingconditionpoissonctrlpointsdialog.h"
#include "ui_gridcreatingconditionpoissonctrlpointsdialog.h"

#include <QFile>
#include <QPixmap>

GridCreatingConditionPoissonCtrlPointsDialog::GridCreatingConditionPoissonCtrlPointsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionPoissonCtrlPointsDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionPoissonCtrlPointsDialog::~GridCreatingConditionPoissonCtrlPointsDialog()
{
	delete ui;
}

void GridCreatingConditionPoissonCtrlPointsDialog::setLocale(const QLocale &locale)
{
	QString fname = QString(":/libs/gridcreatingcondition/poisson/images/%1/imgCtrlCrossSections.png").arg(locale.name());
	QFile file(fname);
	if (! file.exists()) {return;}

	ui->imgLabel->setPixmap(QPixmap(fname));
}

void GridCreatingConditionPoissonCtrlPointsDialog::setMaximum(int max)
{
	ui->numSpinBox->setMaximum(max);
	int num = max / 3;
	ui->numSpinBox->setValue(num);
}

int GridCreatingConditionPoissonCtrlPointsDialog::value() const
{
	return ui->numSpinBox->value();
}
