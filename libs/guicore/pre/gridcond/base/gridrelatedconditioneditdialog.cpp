#include "../../../solverdef/solverdefinitiongridrelatedcondition.h"
#include "gridrelatedconditioneditdialog.h"
#include "ui_gridrelatedconditioneditdialog.h"

#include <QString>

GridRelatedConditionEditDialog::GridRelatedConditionEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridRelatedConditionEditDialog)
{
	m_widget = nullptr;
	m_cancelInhibited = false;
	ui->setupUi(this);
}

GridRelatedConditionEditDialog::~GridRelatedConditionEditDialog()
{
	delete ui;
}

void GridRelatedConditionEditDialog::inhibitCancel(bool inhibited)
{
	ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
	m_cancelInhibited = inhibited;
}

void GridRelatedConditionEditDialog::setWidget(GridRelatedConditionEditWidget* w)
{
	m_widget = w;
	ui->widget->setWidget(w);
	QString label = w->condition()->caption();
	label.append(" :");
	ui->conditionLabel->setText(label);
	adjustSize();
}

void GridRelatedConditionEditDialog::accept()
{
	QVariant v = m_widget->variantValue();
	QDialog::accept();
}

void GridRelatedConditionEditDialog::reject()
{
	if (m_cancelInhibited) {return;}
	QDialog::reject();
}

void GridRelatedConditionEditDialog::setLabel(const QString& label)
{
	ui->label->setText(label);
}

