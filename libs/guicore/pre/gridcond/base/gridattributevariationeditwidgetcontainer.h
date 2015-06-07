#ifndef GRIDRELATEDCONDITIONVARIATIONEDITWIDGETCONTAINER_H
#define GRIDRELATEDCONDITIONVARIATIONEDITWIDGETCONTAINER_H

#include "gridattributevariationeditwidget.h"
#include <QWidget>
#include <QVBoxLayout>

class GridAttributeVariationEditWidgetContainer : public QWidget
{
public:
	GridAttributeVariationEditWidgetContainer(QWidget* parent)
		: QWidget(parent) {
		m_widget = nullptr;
	}
	void setWidget(GridAttributeVariationEditWidget* w) {
		m_widget = w;
		QVBoxLayout* l = new QVBoxLayout();
		l->setMargin(0);
		l->addWidget(w);
		setLayout(l);
	}
	GridAttributeVariationEditWidget* widget() const {return m_widget;}
	QSize sizeHint() const override {return m_widget->sizeHint();}

private:
	GridAttributeVariationEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONEDITWIDGETCONTAINER_H
