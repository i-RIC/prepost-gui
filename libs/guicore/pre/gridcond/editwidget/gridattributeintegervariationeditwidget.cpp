#include "gridattributeintegervariationeditwidget.h"

#include <guibase/widget/integernumbereditwidget.h>

#include <QVBoxLayout>

GridAttributeIntegerVariationEditWidget::GridAttributeIntegerVariationEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond) :
	GridAttributeVariationEditWidgetT<int, vtkIntArray>(parent, cond)
{
	m_widget = new IntegerNumberEditWidget(this);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	if (cond->variantMaximumValue() != "") {
		bool ok;
		int max = cond->variantMaximumValue().toInt(&ok);
		if (ok) {
			m_widget->setMaximum(max);
		}
	}
	if (cond->variantMinimumValue() != "") {
		bool ok;
		int min = cond->variantMinimumValue().toInt(&ok);
		if (ok) {
			m_widget->setMinimum(min);
		}
	}
	setSizePolicy(m_widget->sizePolicy());
}

GridAttributeIntegerVariationEditWidget::~GridAttributeIntegerVariationEditWidget()
{}

QSize GridAttributeIntegerVariationEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridAttributeIntegerVariationEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}

void GridAttributeIntegerVariationEditWidget::getValueFromInnerWidget()
{
	m_value = m_widget->value();
}
