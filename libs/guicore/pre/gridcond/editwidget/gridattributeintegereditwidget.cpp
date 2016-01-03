#include "gridattributeintegereditwidget.h"

#include <guibase/integernumbereditwidget.h>

#include <QVBoxLayout>

GridAttributeIntegerEditWidget::GridAttributeIntegerEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond)
	: GridAttributeEditWidgetT<int>(parent, cond)
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

GridAttributeIntegerEditWidget::~GridAttributeIntegerEditWidget()
{}

void GridAttributeIntegerEditWidget::getValueFromInnerWidget() const
{
	m_value = m_widget->value();
}

void GridAttributeIntegerEditWidget::setupWidget()
{
	// this widget does not allowed "cleared" status.
	m_widget->setValue(m_value);
}

QSize GridAttributeIntegerEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridAttributeIntegerEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}
