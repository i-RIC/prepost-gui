#include "gridcreatingconditiontriangleremeshdialog.h"
#include "ui_gridcreatingconditiontriangleremeshdialog.h"
#include <QMessageBox>
#include <QPushButton>

GridCreatingConditionTriangleRemeshDialog::GridCreatingConditionTriangleRemeshDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionTriangleRemeshDialog)
{
	ui->setupUi(this);
	m_required = false;
}

GridCreatingConditionTriangleRemeshDialog::~GridCreatingConditionTriangleRemeshDialog()
{
	delete ui;
}

void GridCreatingConditionTriangleRemeshDialog::setArea(double area)
{
	ui->areaEdit->setValue(area);
}

double GridCreatingConditionTriangleRemeshDialog::area(){
	return ui->areaEdit->value();
}

void GridCreatingConditionTriangleRemeshDialog::setRequired()
{
	m_required = true;
	QAbstractButton* cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);
	if (cancelButton != 0){
		ui->buttonBox->removeButton(cancelButton);
	}
}

void GridCreatingConditionTriangleRemeshDialog::accept()
{
	if (ui->areaEdit->value() <= 0){
		QMessageBox::warning(this, tr("Warning"), tr("Please specify positive value."));
		return;
	}
	QDialog::accept();
}

void GridCreatingConditionTriangleRemeshDialog::reject()
{
	if (m_required){
		return;
	}
	QDialog::reject();
}
