#include "gridrelatedconditionrealvariationeditwidget.h"
#include <guibase/realnumbereditwidget.h>
#include <QVBoxLayout>

GridRelatedConditionRealVariationEditWidget::GridRelatedConditionRealVariationEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<double>* cond)
	: GridRelatedConditionVariationEditWidgetT<double>(parent, cond)
{
	m_widget = new RealNumberEditWidget(this);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	setSizePolicy(m_widget->sizePolicy());
}

GridRelatedConditionRealVariationEditWidget::~GridRelatedConditionRealVariationEditWidget()
{}

void GridRelatedConditionRealVariationEditWidget::setupWidget()
{
	m_widget->setText("");
}

void GridRelatedConditionRealVariationEditWidget::getValueFromInnerWidget()
{
	if (m_widget->text() == ""){
		m_value = 0;
	}else{
		m_value = m_widget->value();
	}
}

QSize GridRelatedConditionRealVariationEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridRelatedConditionRealVariationEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}
