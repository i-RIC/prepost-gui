#include "qwidgetcontainer.h"

#include <QVBoxLayout>

QWidgetContainer::QWidgetContainer(QWidget* parent) :
	QWidget {parent},
	m_widget {nullptr}
{
	auto l = new QVBoxLayout();
	l->setMargin(0);
	setLayout(l);
}

QWidget* QWidgetContainer::widget() const
{
	return m_widget;
}

void QWidgetContainer::setWidget(QWidget* w)
{
	auto l = layout();

	if (m_widget != nullptr){
		m_widget->hide();
		l->removeWidget(m_widget);
	}

	m_widget = w;
	if (w != nullptr) {
		w->show();
		l->addWidget(w);
	}
}

QSize QWidgetContainer::sizeHint() const
{
	if (m_widget == nullptr) {
		return QSize();
	}

	return m_widget->sizeHint();
}
