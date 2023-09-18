#include "gridattributerealvariationeditwidget.h"

#include <guibase/widget/realnumbereditwidget.h>

#include <QVBoxLayout>

GridAttributeRealVariationEditWidget::GridAttributeRealVariationEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond) :
	GridAttributeVariationEditWidgetT<double, vtkDoubleArray>(parent, cond)
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

void GridAttributeRealVariationEditWidget::setMode(Mode mode)
{
	if (mode == Ratio) {
		m_widget->setValue(1.0);
	}
	GridAttributeVariationEditWidget::setMode(mode);
}

QSize GridAttributeRealVariationEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridAttributeRealVariationEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}

void GridAttributeRealVariationEditWidget::getValueFromInnerWidget()
{
	if (m_widget->text() == "") {
		m_value = 0;
	} else {
		m_value = m_widget->value();
	}
}
