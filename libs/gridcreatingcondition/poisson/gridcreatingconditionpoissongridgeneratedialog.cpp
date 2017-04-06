#include "gridcreatingconditionpoissongridgeneratedialog.h"
#include "ui_gridcreatingconditionpoissongridgeneratedialog.h"

GridCreatingConditionPoissonGridGenerateDialog::GridCreatingConditionPoissonGridGenerateDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionPoissonGridGenerateDialog)
{
	ui->setupUi(this);
	connect(ui->nISpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateDI()));
	connect(ui->nJSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateDJ()));
}

GridCreatingConditionPoissonGridGenerateDialog::~GridCreatingConditionPoissonGridGenerateDialog()
{
	delete ui;
}

int GridCreatingConditionPoissonGridGenerateDialog::iDiv() const
{
	return ui->nISpinBox->value();
}

void GridCreatingConditionPoissonGridGenerateDialog::setIDiv(int div)
{
	ui->nISpinBox->setValue(div);
	updateDI();
}

int GridCreatingConditionPoissonGridGenerateDialog::jDiv() const
{
	return ui->nJSpinBox->value();
}

void GridCreatingConditionPoissonGridGenerateDialog::setJDiv(int div)
{
	ui->nJSpinBox->setValue(div);
	updateDJ();
}

void GridCreatingConditionPoissonGridGenerateDialog::setILength(double len)
{
	m_iLength = len;
}

void GridCreatingConditionPoissonGridGenerateDialog::setJLength(double len)
{
	m_jLength = len;
}

void GridCreatingConditionPoissonGridGenerateDialog::updateDI()
{
	double d = m_iLength / (iDiv() - 1);
	ui->dIValue->setText(QString::number(d));
}

void GridCreatingConditionPoissonGridGenerateDialog::updateDJ()
{
	double d = m_jLength / (jDiv() - 1);
	ui->dJValue->setText(QString::number(d));
}
