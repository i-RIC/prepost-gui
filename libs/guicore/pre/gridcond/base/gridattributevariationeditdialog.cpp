#include "ui_gridattributevariationeditdialog.h"

#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "gridattributevariationeditdialog.h"

#include <QString>

GridAttributeVariationEditDialog::GridAttributeVariationEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridAttributeVariationEditDialog)
{
	m_widget = nullptr;
	ui->setupUi(this);
}

GridAttributeVariationEditDialog::~GridAttributeVariationEditDialog()
{
	delete ui;
}

void GridAttributeVariationEditDialog::setWidget(GridAttributeVariationEditWidget* w)
{
	m_widget = w;
	ui->widget->setWidget(w);
	QString label = w->gridAttribute()->caption();
	label.append(" :");
	ui->conditionLabel->setText(label);
	adjustSize();
}

void GridAttributeVariationEditDialog::accept()
{
	QVariant v = m_widget->variantValue();
	QDialog::accept();
}

void GridAttributeVariationEditDialog::reject()
{
	QDialog::reject();
}

void GridAttributeVariationEditDialog::setLabel(const QString& label)
{
	ui->label->setText(label);
}
