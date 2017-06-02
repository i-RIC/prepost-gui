#ifndef GRIDCOMPLEXCONDITIONGROUPEDITWIDGETCONTAINER_H
#define GRIDCOMPLEXCONDITIONGROUPEDITWIDGETCONTAINER_H

#include <QWidget>

class GridComplexConditionGroup;

class QVBoxLayout;

class GridComplexConditionGroupEditWidgetContainer : public QWidget
{
public:
	GridComplexConditionGroupEditWidgetContainer(QWidget* parent);
	~GridComplexConditionGroupEditWidgetContainer();

	void setGroup(GridComplexConditionGroup* group);

private:
	GridComplexConditionGroup* m_group;
	QVBoxLayout* m_layout;
};

#endif // GRIDCOMPLEXCONDITIONGROUPEDITWIDGETCONTAINER_H
