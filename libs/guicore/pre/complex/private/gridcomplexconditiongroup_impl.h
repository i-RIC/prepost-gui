#ifndef GRIDCOMPLEXCONDITIONGROUP_IMPL_H
#define GRIDCOMPLEXCONDITIONGROUP_IMPL_H

#include "../gridcomplexconditiongroup.h"
#include "../../../project/inputcond/inputconditioncontainerinteger.h"
#include "../../../project/inputcond/inputconditioncontainerstring.h"
#include "../../../project/inputcond/inputconditioncontainerset.h"
#include "../../../project/inputcond/inputconditionwidgetset.h"

class InputConditionPage;

class GridComplexConditionGroup::Impl
{
public:
	Impl(SolverDefinition* def, const QDomElement& elem);
	~Impl();

	InputConditionPage* m_page;
	InputConditionWidgetSet m_widgetSet;

	InputConditionContainerString m_caption;
	InputConditionContainerString m_color;
	InputConditionContainerInteger m_isDefault;
	InputConditionContainerSet m_containerSet;
};

#endif // GRIDCOMPLEXCONDITIONGROUP_IMPL_H
