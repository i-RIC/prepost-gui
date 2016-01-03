#ifndef GRIDCOMPLEXCONDITIONWIDGETCONTAINER_H
#define GRIDCOMPLEXCONDITIONWIDGETCONTAINER_H

#include <QWidget>

class GridComplexConditionWidgetContainer : public QWidget
{
	Q_OBJECT

public:
	GridComplexConditionWidgetContainer(QWidget* w);
	~GridComplexConditionWidgetContainer();

	void setWidget(QWidget* widget);

private:
	QWidget* m_widget;
};

#endif // GRIDCOMPLEXCONDITIONWIDGETCONTAINER_H
