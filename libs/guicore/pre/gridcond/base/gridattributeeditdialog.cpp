#include "ui_gridattributeeditdialog.h"

#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "gridattributeeditdialog.h"

#include <QString>

GridAttributeEditDialog::GridAttributeEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridAttributeEditDialog)
{
	m_widget = nullptr;
	m_cancelInhibited = false;
	ui->setupUi(this);
}

GridAttributeEditDialog::~GridAttributeEditDialog()
{
	delete ui;
}

void GridAttributeEditDialog::inhibitCancel(bool inhibited)
{
	ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
	m_cancelInhibited = inhibited;
}

void GridAttributeEditDialog::setWidget(GridAttributeEditWidget* w)
{
	m_widget = w;
	ui->widget->setWidget(w);
	QString label = w->condition()->caption();
	label.append(" :");
	ui->conditionLabel->setText(label);
	adjustSize();
}

void GridAttributeEditDialog::accept()
{
	QVariant v = m_widget->variantValue();
	QDialog::accept();
}

void GridAttributeEditDialog::reject()
{
	if (m_cancelInhibited) {return;}
	QDialog::reject();
}

void GridAttributeEditDialog::setLabel(const QString& label)
{
	ui->label->setText(label);
}

