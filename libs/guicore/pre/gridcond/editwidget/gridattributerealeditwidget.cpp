#include "gridattributerealeditwidget.h"

#include <guibase/realnumbereditwidget.h>

#include <QVBoxLayout>

GridAttributeRealEditWidget::GridAttributeRealEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond) :
	GridAttributeEditWidgetT<double>(parent, cond)
{
	m_widget = new RealNumberEditWidget(this);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	setSizePolicy(m_widget->sizePolicy());
}

GridAttributeRealEditWidget::~GridAttributeRealEditWidget()
{}

void GridAttributeRealEditWidget::setupWidget()
{
	if (isValueCleared()) {
		m_widget->setText("");
	} else {
		m_widget->setValue(m_value);
	}
}

void GridAttributeRealEditWidget::getValueFromInnerWidget() const
{
	if (m_widget->text() == "") {
		setValueSelected(false);
	} else {
		setValueSelected(true);
		m_value = m_widget->value();
	}
}

QWidget* GridAttributeRealEditWidget::editWidget() const
{
	return m_widget;
}
