#include "marginwidget.h"

#include <QGridLayout>

MarginWidget::MarginWidget(QWidget* parent) :
	QWidget(parent)
{
	m_marginPixels = 5;
}

int MarginWidget::marginPixels() const
{
	return m_marginPixels;
}

void MarginWidget::setMarginPixels(int m)
{
	m_marginPixels = m;
}

QWidget* MarginWidget::widget() const
{
	return m_widget;
}

void MarginWidget::setWidget(QWidget* w)
{
	QGridLayout* layout = new QGridLayout(this);
	layout->setContentsMargins(m_marginPixels, m_marginPixels, m_marginPixels, m_marginPixels);
	layout->addWidget(w, 0, 0, 1, 1);
	setLayout(layout);
}
