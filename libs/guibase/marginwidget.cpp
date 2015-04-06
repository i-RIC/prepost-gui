#include "marginwidget.h"
#include <QGridLayout>

MarginWidget::MarginWidget(QWidget *parent) :
	QWidget(parent)
{
	m_margin = 5;
}

void MarginWidget::setWidget(QWidget* w)
{
	QGridLayout* layout = new QGridLayout(this);
	layout->setContentsMargins(m_margin, m_margin, m_margin, m_margin);
	layout->addWidget(w, 0, 0, 1, 1);
	setLayout(layout);
}
