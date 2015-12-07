#ifndef GRIDATTRIBUTEEDITWIDGETCONTAINER_H
#define GRIDATTRIBUTEEDITWIDGETCONTAINER_H

#include "gridattributeeditwidget.h"
#include <QWidget>
#include <QVBoxLayout>

class GridAttributeEditWidgetContainer : public QWidget
{

public:
	GridAttributeEditWidgetContainer(QWidget* parent)
		: QWidget(parent) {
		m_widget = nullptr;
	}
	void setWidget(GridAttributeEditWidget* w) {
		m_widget = w;
		QVBoxLayout* l = new QVBoxLayout();
		l->setMargin(0);
		l->addWidget(w);
		setLayout(l);
	}
	GridAttributeEditWidget* widget() {return m_widget;}
	QSize sizeHint() const override {
		return m_widget->sizeHint();
	}

private:
	GridAttributeEditWidget* m_widget;
};

#endif // GRIDATTRIBUTEEDITWIDGETCONTAINER_H
