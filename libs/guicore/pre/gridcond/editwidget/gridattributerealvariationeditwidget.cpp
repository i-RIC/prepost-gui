#include "gridattributerealvariationeditwidget.h"

#include <guibase/realnumbereditwidget.h>

#include <QVBoxLayout>

GridAttributeRealVariationEditWidget::GridAttributeRealVariationEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond)
	: GridAttributeVariationEditWidgetT<double>(parent, cond)
{
	m_widget = new RealNumberEditWidget(this);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	setSizePolicy(m_widget->sizePolicy());
}

GridAttributeRealVariationEditWidget::~GridAttributeRealVariationEditWidget()
{}

void GridAttributeRealVariationEditWidget::setupWidget()
{
	m_widget->setText("");
}

void GridAttributeRealVariationEditWidget::getValueFromInnerWidget()
{
	if (m_widget->text() == "") {
		m_value = 0;
	} else {
		m_value = m_widget->value();
	}
}

QSize GridAttributeRealVariationEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridAttributeRealVariationEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}
