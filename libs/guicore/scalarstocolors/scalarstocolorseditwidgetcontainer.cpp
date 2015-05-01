#include "scalarstocolorseditwidgetcontainer.h"

#include "scalarstocolorseditwidget.h"
#include <QWidget>
#include <QVBoxLayout>

ScalarsToColorsEditWidgetContainer::ScalarsToColorsEditWidgetContainer(QWidget* parent)
	: QWidget(parent)
{
	m_widget = 0;
}

void ScalarsToColorsEditWidgetContainer::setWidget(ScalarsToColorsEditWidget* w)
{
	m_widget = w;
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(w);
	setLayout(l);
}

QSize ScalarsToColorsEditWidgetContainer::sizeHint() const
{
	return m_widget->sizeHint();
}

void ScalarsToColorsEditWidgetContainer::save()
{
	m_widget->save();
}
