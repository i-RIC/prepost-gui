#ifndef GRIDRELATEDCONDITIONEDITWIDGETCONTAINER_H
#define GRIDRELATEDCONDITIONEDITWIDGETCONTAINER_H

#include "gridrelatedconditioneditwidget.h"
#include <QWidget>
#include <QVBoxLayout>

class GridRelatedConditionEditWidgetContainer : public QWidget
{
public:
	GridRelatedConditionEditWidgetContainer(QWidget* parent)
		: QWidget(parent) {
		m_widget = nullptr;
	}
	void setWidget(GridRelatedConditionEditWidget* w) {
		m_widget = w;
		QVBoxLayout* l = new QVBoxLayout();
		l->setMargin(0);
		l->addWidget(w);
		setLayout(l);
	}
	GridRelatedConditionEditWidget* widget() {return m_widget;}
	QSize sizeHint() const {
		return m_widget->sizeHint();
	}
private:
	GridRelatedConditionEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONEDITWIDGETCONTAINER_H
