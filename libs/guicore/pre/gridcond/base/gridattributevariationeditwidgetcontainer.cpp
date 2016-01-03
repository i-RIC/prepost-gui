#include "gridattributevariationeditwidget.h"
#include "gridattributevariationeditwidgetcontainer.h"

#include <QVBoxLayout>

GridAttributeVariationEditWidgetContainer::GridAttributeVariationEditWidgetContainer(QWidget* parent) :
	QWidget {parent},
	m_widget {nullptr}
{}

GridAttributeVariationEditWidget* GridAttributeVariationEditWidgetContainer::widget() const
{
	return m_widget;
}

void GridAttributeVariationEditWidgetContainer::setWidget(GridAttributeVariationEditWidget* w)
{
	m_widget = w;
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(w);
	setLayout(l);
}

QSize GridAttributeVariationEditWidgetContainer::sizeHint() const
{
	return m_widget->sizeHint();
}
