#ifndef GRIDCOMPLEXCONDITIONWIDGETCONTAINER_H
#define GRIDCOMPLEXCONDITIONWIDGETCONTAINER_H

#include <QWidget>

class GridComplexConditionWidgetContainer : public QWidget
{
	Q_OBJECT

public:
	/// Constructor
	GridComplexConditionWidgetContainer(QWidget* w);
	/// Destructor
	~GridComplexConditionWidgetContainer();
	void setWidget(QWidget* widget);

private:
	QWidget* m_widget;
};

#endif // GRIDCOMPLEXCONDITIONWIDGETCONTAINER_H
