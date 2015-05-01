#include "gridrelatedconditionintegervariationeditwidget.h"
#include <guibase/integernumbereditwidget.h>
#include <QVBoxLayout>

GridRelatedConditionIntegerVariationEditWidget::GridRelatedConditionIntegerVariationEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<int>* cond)
	: GridRelatedConditionVariationEditWidgetT<int>(parent, cond)
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

GridRelatedConditionIntegerVariationEditWidget::~GridRelatedConditionIntegerVariationEditWidget()
{}

void GridRelatedConditionIntegerVariationEditWidget::getValueFromInnerWidget()
{
	m_value = m_widget->value();
}

void GridRelatedConditionIntegerVariationEditWidget::setupWidget()
{
	// this widget does not allowed "cleared" status.
	m_widget->setValue(m_value);
}

QSize GridRelatedConditionIntegerVariationEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridRelatedConditionIntegerVariationEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}
