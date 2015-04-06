#include "gridcomplexconditionwidgetcontainer.h"

#include <QHBoxLayout>

GridComplexConditionWidgetContainer::GridComplexConditionWidgetContainer(QWidget* w)
	: QWidget(w)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	setLayout(layout);

	m_widget = 0;
}

GridComplexConditionWidgetContainer::~GridComplexConditionWidgetContainer()
{

}

void GridComplexConditionWidgetContainer::setWidget(QWidget* widget)
{
	QLayout* l = layout();
	if (m_widget != 0){
		l->removeWidget(m_widget);
		m_widget->hide();
	}
	m_widget = widget;
	l->addWidget(widget);
	widget->show();
}
