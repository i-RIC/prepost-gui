#ifndef GRIDCOMPLEXCONDITIONGROUP_IMPL_H
#define GRIDCOMPLEXCONDITIONGROUP_IMPL_H

#include "../gridcomplexconditiongroup.h"

class InputConditionWidgetSet;
class InputConditionPage;

class GridComplexConditionGroup::Impl
{
public:
	Impl(SolverDefinition* def, const QDomElement& elem);
	~Impl();

	InputConditionPage* m_page;
	InputConditionWidgetSet* m_widgetSet;
	InputConditionContainerSet* m_containerSet;
};

#endif // GRIDCOMPLEXCONDITIONGROUP_IMPL_H
