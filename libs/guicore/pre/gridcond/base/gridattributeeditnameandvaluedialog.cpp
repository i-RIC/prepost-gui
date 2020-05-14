#include "gridattributeeditnameandvaluedialog.h"
#include "ui_gridattributeeditnameandvaluedialog.h"

#include "../../../solverdef/solverdefinitiongridattribute.h"

GridAttributeEditNameAndValueDialog::GridAttributeEditNameAndValueDialog(QWidget *parent) :
	QDialog(parent),
	m_cancelDisabled {false},
	ui {new Ui::GridAttributeEditNameAndValueDialog}
{
	ui->setupUi(this);
}

GridAttributeEditNameAndValueDialog::~GridAttributeEditNameAndValueDialog()
{
	delete ui;
}

GridAttributeEditWidget* GridAttributeEditNameAndValueDialog::widget() const
{
	return m_widget;
}

void GridAttributeEditNameAndValueDialog::setWidget(GridAttributeEditWidget* w)
{
	m_widget = w;
	ui->widget->setWidget(w);
	QString label = w->gridAttribute()->caption();
	label.append(" :");
	ui->conditionLabel->setText(label);
	adjustSize();
}

void GridAttributeEditNameAndValueDialog::clearValue()
{
	m_widget->clearValue();
}

void GridAttributeEditNameAndValueDialog::disableCancel()
{
	ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
	m_cancelDisabled = true;
}

bool GridAttributeEditNameAndValueDialog::isValueSelected() const
{
	return m_widget->isValueSelected();
}

void GridAttributeEditNameAndValueDialog::setName(const QString& name)
{
	ui->nameEdit->setText(name);
}

QString GridAttributeEditNameAndValueDialog::name() const
{
	return ui->nameEdit->text();
}

void GridAttributeEditNameAndValueDialog::setVariantValue(const QVariant& v)
{
	m_widget->setVariantValue(v);
}

QVariant GridAttributeEditNameAndValueDialog::variantValue() const
{
	return m_widget->variantValue();
}

void GridAttributeEditNameAndValueDialog::accept()
{
	QVariant v = m_widget->variantValue();
	QDialog::accept();
}

void GridAttributeEditNameAndValueDialog::reject()
{
	if (m_cancelDisabled) {return;}
	QDialog::reject();
}
