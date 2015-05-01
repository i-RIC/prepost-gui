#include "../../../solverdef/solverdefinitiongridrelatedcondition.h"
#include "gridrelatedconditionvariationeditdialog.h"
#include "ui_gridrelatedconditionvariationeditdialog.h"

#include <QString>

GridRelatedConditionVariationEditDialog::GridRelatedConditionVariationEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridRelatedConditionVariationEditDialog)
{
	m_widget = nullptr;
	ui->setupUi(this);
}

GridRelatedConditionVariationEditDialog::~GridRelatedConditionVariationEditDialog()
{
	delete ui;
}

void GridRelatedConditionVariationEditDialog::setWidget(GridRelatedConditionVariationEditWidget* w)
{
	m_widget = w;
	ui->widget->setWidget(w);
	QString label = w->condition()->caption();
	label.append(" :");
	ui->conditionLabel->setText(label);
	adjustSize();
}

void GridRelatedConditionVariationEditDialog::accept()
{
	QVariant v = m_widget->variantValue();
	QDialog::accept();
}

void GridRelatedConditionVariationEditDialog::reject()
{
	QDialog::reject();
}

void GridRelatedConditionVariationEditDialog::setLabel(const QString& label)
{
	ui->label->setText(label);
}
