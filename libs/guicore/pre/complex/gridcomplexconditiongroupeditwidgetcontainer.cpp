#include "gridcomplexconditiongroupeditwidgetcontainer.h"
#include "gridcomplexconditiongroup.h"

#include <QVBoxLayout>

GridComplexConditionGroupEditWidgetContainer::GridComplexConditionGroupEditWidgetContainer(QWidget* parent) :
	QWidget {parent}
{}

GridComplexConditionGroupEditWidgetContainer::~GridComplexConditionGroupEditWidgetContainer()
{
	auto w = m_group->widget();
	w->hide();
	w->setParent(nullptr);
}

void GridComplexConditionGroupEditWidgetContainer::setGroup(GridComplexConditionGroup* group)
{
	m_group = group;

	auto w = m_group->widget();
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(w);
	w->show();
	setLayout(l);
}
