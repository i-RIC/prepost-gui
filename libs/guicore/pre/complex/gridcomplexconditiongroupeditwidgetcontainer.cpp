#include "gridcomplexconditiongroupeditwidgetcontainer.h"
#include "gridcomplexconditiongroup.h"

#include <QVBoxLayout>

GridComplexConditionGroupEditWidgetContainer::GridComplexConditionGroupEditWidgetContainer(QWidget* parent) :
	QWidget {parent},
	m_group {nullptr},
	m_layout {new QVBoxLayout(this)}
{
	m_layout->setMargin(0);
	setLayout(m_layout);
}

GridComplexConditionGroupEditWidgetContainer::~GridComplexConditionGroupEditWidgetContainer()
{
	if (m_group == nullptr) {return;}

	auto w = m_group->widget();
	w->setParent(nullptr);
	m_layout->removeWidget(w);
}

void GridComplexConditionGroupEditWidgetContainer::setGroup(GridComplexConditionGroup* group)
{
	while (m_layout->count() > 0) {
		auto item = m_layout->itemAt(0);
		m_layout->removeItem(item);
		QWidget* w = item->widget();
		if (w != nullptr) {
			w->hide();
			w->setParent(nullptr);
		}
	}
	m_group = group;
	if (m_group == nullptr) {return;}

	auto w = m_group->widget();
	w->setParent(this);
	w->show();
	m_layout->addWidget(w);
}
