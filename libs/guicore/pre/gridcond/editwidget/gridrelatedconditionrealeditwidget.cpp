#include "gridrelatedconditionrealeditwidget.h"

#include <guibase/realnumbereditwidget.h>

#include <QVBoxLayout>

GridRelatedConditionRealEditWidget::GridRelatedConditionRealEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<double>* cond)
	: GridRelatedConditionEditWidgetT<double>(parent, cond)
{
	m_widget = new RealNumberEditWidget(this);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	setSizePolicy(m_widget->sizePolicy());
}

GridRelatedConditionRealEditWidget::~GridRelatedConditionRealEditWidget()
{}

void GridRelatedConditionRealEditWidget::setupWidget()
{
	if (m_valueCleared) {
		m_widget->setText("");
	} else {
		m_widget->setValue(m_value);
	}
}

void GridRelatedConditionRealEditWidget::getValueFromInnerWidget()
{
	if (m_widget->text() == "") {
		m_valueSelected = false;
	} else {
		m_valueSelected = true;
		m_value = m_widget->value();
	}
}

QSize GridRelatedConditionRealEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridRelatedConditionRealEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}
